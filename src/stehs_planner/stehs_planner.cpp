#include "stehs_planner.hpp"


// constructor
StehsPlanner::StehsPlanner() {}


/*std::vector<CircleNode>
StehsPlanner::Expand(CircleNodePtr current)
{
	//stehs_planner.goal;

}*/

double
StehsPlanner::Distance(double ax, double ay, double bx, double by)
{
	double dx = ax - bx;
	double dy = ay - by;
	return std::sqrt(dx * dx + dy * dy);
}


double
StehsPlanner::Distance(const carmen_ackerman_traj_point_t &a, const carmen_ackerman_traj_point_t &b)
{
	double dx = a.x - b.x;
	double dy = a.y - b.y;
	double distance = sqrt(dx * dx + dy * dy);

	return (distance);
}


double
StehsPlanner::Distance2(const carmen_ackerman_traj_point_t &a, const carmen_ackerman_traj_point_t &b)
{
	double dx = a.x - b.x;
	double dy = a.y - b.y;
	double distance = (dx * dx + dy * dy);

	return (distance);
}


carmen_point_t
traj_to_point_t(carmen_ackerman_traj_point_t traj)
{
	carmen_point_t point;

	point.x = traj.x;
	point.y = traj.y;
	point.theta = traj.theta;

	return (point);
}

double
StehsPlanner::ObstacleDistance(double x, double y)
{
	carmen_point_t p;

	p.x = x;
	p.y = y;

	return (carmen_obstacle_avoider_distance_from_global_point_to_obstacle(&p, distance_map));
}

double
StehsPlanner::ObstacleDistance(const carmen_ackerman_traj_point_t &point)
{
	carmen_point_t p;
	p = traj_to_point_t(point);

	return (carmen_obstacle_avoider_distance_from_global_point_to_obstacle(&p, distance_map));
}


std::list<CircleNode>
StehsPlanner::BuildCirclePath(CircleNodePtr node)
{
	std::list<CircleNode> temp_circle_path;

	while (node != NULL)
	{
		temp_circle_path.push_front(*node);

		node = node->parent;
	}

	return(temp_circle_path);
}

bool
StehsPlanner::Exist(CircleNodePtr current, std::vector<CircleNodePtr> &closed_set)
{
	std::vector<CircleNodePtr>::iterator it = closed_set.begin();
	std::vector<CircleNodePtr>::iterator end = closed_set.end();

	while (it != end)
	{
		if (current->circle.Overlap((*it)->circle, MAX_OVERLAP_FACTOR))
			return true;

		it++;
	}

	return false;
}


std::vector<CircleNodePtr>
StehsPlanner::Expand(CircleNodePtr current)
{

	std::vector<CircleNodePtr> children;

	// Considering a 2m radius parent circle that involves the car we
	// want 32 children circles
	unsigned int children_amount = (unsigned int) (16.0*current->circle.radius);

	double theta = 2.0*M_PI / (double) children_amount;
	double c = std::cos(theta);
	double s = std::sin(theta);
	double t;

	// the parent coordinates
	double px = current->circle.x;
	double py = current->circle.y;

	// the parent cost
	double pg = current->g;

	// the parent radius
	double pr = current->circle.radius;

	//we start at angle = 0
	double x = pr;
	double y = 0.0;

	for(unsigned int i = 0; i < children_amount; i++)
	{
		double nx = x + px;
		double ny = y + py;

		double obstacle_distance = ObstacleDistance(nx, ny);

		if (obstacle_distance > robot_config.width * 0.5) {

			// create the next child
			// TODO verificar se é possível remover filhos com overlap
			children.push_back(new CircleNode(nx, ny, obstacle_distance, pg + pr, pg + pr + Distance(nx, ny, goal.x, goal.y), current));

		}

		//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}

	return children;
}


std::list<CircleNode>
StehsPlanner::SpaceExploration(CircleNodePtr start_node, CircleNodePtr goal_node)
{
	std::list<CircleNode> temp_circle_path;

	start_node->g = 0.0;
	start_node->f = Distance(start_node->circle.x, start_node->circle.y, goal_node->circle.x, goal_node->circle.y);

	goal_node->g = goal_node->f = DBL_MAX;

	// create the priority queue
	std::priority_queue<CircleNodePtr, std::vector<CircleNodePtr>, CircleNodePtrComparator> open_set;

	std::vector<CircleNodePtr> closed_set;

	open_set.push(start_node);

	while (!open_set.empty())
	{
		// get the circle wich is the closest to the goal node
		CircleNodePtr current = open_set.top();
		open_set.pop();

		if (goal_node->f < current->f)
		{
			temp_circle_path = BuildCirclePath(goal_node);

			while(!open_set.empty())
			{
				CircleNodePtr tmp = open_set.top();

				open_set.pop();

				delete tmp;
			}
			break;
		}
		else if (!Exist(current, closed_set))
		{
			std::vector<CircleNodePtr> children = Expand(current);

			// iterator
			std::vector<CircleNodePtr>::iterator it = children.begin();
			std::vector<CircleNodePtr>::iterator end = children.end();

			while (it != end)
			{
				open_set.push(*it);

				// update the iterator
				++it;

			}

			if (current->circle.Overlap(goal_node->circle, MIN_OVERLAP_FACTOR))
			{

				if (current->f < goal_node->g)
				{
					goal_node->g = current->f;
					goal_node->parent = current;
				}
			}
			closed_set.push_back(current);
		}
	}

	while(!closed_set.empty())
	{
		CircleNodePtr tmp = closed_set.back();

		closed_set.pop_back();

		delete tmp;
	}

	return (temp_circle_path);
}


int
StehsPlanner::FindClosestRDDFIndex()
{
	double current_distance = 0.0;
	double previous_distance = Distance(start, goal_list_message->poses[0]);
	int i;

	for (i = 1; i < goal_list_message->number_of_poses; i++)
	{
		current_distance = Distance(start, goal_list_message->poses[i]);

		if (current_distance > previous_distance)
			break;
	}

	return (i - 1);
}


int
StehsPlanner::FindNextRDDFIndex(double radius_2, int current_rddf_index)
{
	int i = current_rddf_index + 1;

	while (Distance2(goal_list_message->poses[current_rddf_index], goal_list_message->poses[i]) < radius_2)
	{
		i++;
	}

	return (i - 1);
}


int
StehsPlanner::FindNextRDDFFreeIndex(int current_rddf_index)
{

	double obstacle_distance;

	do
	{
		obstacle_distance = ObstacleDistance(goal_list_message->poses[current_rddf_index++]);
	} while (obstacle_distance < robot_config.width * 0.5);

	return (current_rddf_index);
}


void
StehsPlanner::ConnectCirclePathGaps()
{
	std::list<CircleNode> temp_circle_path;

	std::list<CircleNode>::iterator it = circle_path.begin();
	std::list<CircleNode>::iterator previous_it = it;
	++it;

	while (it != circle_path.end())
	{
		if(!it->circle.Overlap(previous_it->circle, MIN_OVERLAP_FACTOR))
		{
			temp_circle_path = SpaceExploration(&(*previous_it), &(*it));
			if(!temp_circle_path.empty())
				circle_path.splice(previous_it,temp_circle_path);
			else
			{
				// TODO O que fazer?
			}
		}
		previous_it = it;
		++it;
	}
}


void
StehsPlanner::RDDFSpaceExploration()
{
	//TODO verificar se os dados para os calculos estao disponiveis, se nao, nao fazer nada
	// create the start circle node
	CircleNodePtr start_node =  new CircleNode(start.x, start.y, ObstacleDistance(start), 0.0, 0.0, nullptr);

	// create the goal circle node
	CircleNodePtr goal_node = new CircleNode(goal.x, goal.y, ObstacleDistance(goal), DBL_MAX, DBL_MAX, nullptr);

	int current_rddf_index = FindClosestRDDFIndex();

	CircleNodePtr current_node = new CircleNode(goal_list_message->poses[current_rddf_index].x, goal_list_message->poses[current_rddf_index].y, ObstacleDistance(goal_list_message->poses[current_rddf_index]), DBL_MAX, DBL_MAX, nullptr);

	circle_path.push_back(*start_node);

//	if (current_node->circle.Overlap(start_node->circle, 0.5))
//	{
//		circle_path.push_back(*start_node);
//		delete current_node;
//		current_node = start_node;
//	}
//	else
//	{
//		SpaceExploration(start_node, current_node);
//	}

	double obstacle_distance = 0.0;

	while (!current_node->circle.Overlap(goal_node->circle, MIN_OVERLAP_FACTOR))
	{
		// We decided to use radius^2 to optimize the Distance computation
		current_rddf_index = FindNextRDDFIndex(current_node->circle.radius * current_node->circle.radius, current_rddf_index);

		obstacle_distance = ObstacleDistance(goal_list_message->poses[current_rddf_index]);
		if (obstacle_distance > robot_config.width * 0.5)
		{
			current_node = new CircleNode(goal_list_message->poses[current_rddf_index].x, goal_list_message->poses[current_rddf_index].y, obstacle_distance, 0.0, 0.0, nullptr);
		}
		else
		{
			current_rddf_index = FindNextRDDFFreeIndex(current_rddf_index);
			obstacle_distance = ObstacleDistance(goal_list_message->poses[current_rddf_index]);

			current_node = new CircleNode(goal_list_message->poses[current_rddf_index].x, goal_list_message->poses[current_rddf_index].y, obstacle_distance, 0.0, 0.0, nullptr);
			//SpaceExploration(current_node, next_free_node);
		}
		circle_path.push_back(*current_node);
	}


}
//
//void
//StehsPlanner::SpaceTimeExploration() {}
//
////
//void
//StehsPlanner::HeuristicSearch() {}
//
//
////
//std::list<carmen_ackerman_motion_command_t>
//BuildPath();
