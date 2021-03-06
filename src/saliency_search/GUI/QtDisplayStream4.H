/*!@file GUI/QtDisplayStream.H FrameOstream subclass that displays images in a QWidgetStack */

// //////////////////////////////////////////////////////////////////// //
// The iLab Neuromorphic Vision C++ Toolkit - Copyright (C) 2000-2005   //
// by the University of Southern California (USC) and the iLab at USC.  //
// See http://iLab.usc.edu for information about this project.          //
// //////////////////////////////////////////////////////////////////// //
// Major portions of the iLab Neuromorphic Vision Toolkit are protected //
// under the U.S. patent ``Computation of Intrinsic Perceptual Saliency //
// in Visual Environments, and Applications'' by Christof Koch and      //
// Laurent Itti, California Institute of Technology, 2001 (patent       //
// pending; application number 09/912,225 filed July 23, 2001; see      //
// http://pair.uspto.gov/cgi-bin/final/home.pl for current status).     //
// //////////////////////////////////////////////////////////////////// //
// This file is part of the iLab Neuromorphic Vision C++ Toolkit.       //
//                                                                      //
// The iLab Neuromorphic Vision C++ Toolkit is free software; you can   //
// redistribute it and/or modify it under the terms of the GNU General  //
// Public License as published by the Free Software Foundation; either  //
// version 2 of the License, or (at your option) any later version.     //
//                                                                      //
// The iLab Neuromorphic Vision C++ Toolkit is distributed in the hope  //
// that it will be useful, but WITHOUT ANY WARRANTY; without even the   //
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR      //
// PURPOSE.  See the GNU General Public License for more details.       //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with the iLab Neuromorphic Vision C++ Toolkit; if not, write   //
// to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,   //
// Boston, MA 02111-1307 USA.                                           //
// //////////////////////////////////////////////////////////////////// //
//
// Primary maintainer for this file: Rob Peters <rjpeters at usc dot edu>
// $HeadURL: svn://isvn.usc.edu/software/invt/trunk/saliency/src/GUI/QtDisplayStream4.H $
// $Id: QtDisplayStream4.H 12962 2010-03-06 02:13:53Z irock $
//

#ifndef GUI_QTDISPLAYSTREAM4_H_DEFINED
#define GUI_QTDISPLAYSTREAM4_H_DEFINED

#include "Component/ModelParam.H"
#include "Image/Dims.H"
#include "Transport/FrameOstream.H"

#include <QtGui/QApplication>

class QtImageStack;
class EventFilt;

//! FrameOstream subclass that displays images in a QImageStack
class QtDisplayStream : public FrameOstream
{
public:
  QtDisplayStream(OptionManager& mgr,
                  const std::string& descrName = "QtDisplayStream",
                  const std::string& tagName = "QtDisplayStream");

  virtual ~QtDisplayStream();

  //! Override the base version so that we can trap --qt-echo options
  virtual void paramChanged(ModelParamBase* const param,
                            const bool valueChanged,
                            ParamClient::ChangeStatus* status);

  virtual bool setFrameNumber(int n);

  virtual void writeFrame(const GenericFrame& frame,
                          const std::string& shortname,
                          const FrameInfo& auxinfo);

  //! Returns true if the user has clicked the close button
  virtual bool isVoid() const;

  virtual void closeStream(const std::string& shortname);

  //! Returns true if the user has clicked the close button
  bool isClosed() const;

  //! Run our QApplication in a thread
  void* run();

protected:
  friend class EventFilt;

  //!< get started
  virtual void start1();

  //! catch --in-echo command-line options
  OModelParam<std::string> itsQtEcho;

  OModelParam<Dims> itsPreferredDims;
  OModelParam<Dims> itsPreferredMaxDims;

  OModelParam<bool> itsTestMode;

private:
  QtImageStack* itsWidget;
  EventFilt *itsFilter;
  bool itsShown;
  int itsFrameNumber;

  nub::soft_ref<FrameOstream> itsEcho;
  bool itsReady;

  pthread_t runner;
};

// ######################################################################
/* So things look consistent in everyone's emacs... */
/* Local Variables: */
/* mode: c++ */
/* indent-tabs-mode: nil */
/* End: */

#endif // GUI_QTDISPLAYSTREAM4_H_DEFINED
