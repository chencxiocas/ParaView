/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkKWDialog.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2000-2001 Kitware Inc. 469 Clifton Corporate Parkway,
Clifton Park, NY, 12065, USA.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * Neither the name of Kitware nor the names of any contributors may be used
   to endorse or promote products derived from this software without specific 
   prior written permission.

 * Modified source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include "vtkKWApplication.h"
#include "vtkKWDialog.h"
#include "vtkObjectFactory.h"



//------------------------------------------------------------------------------
vtkKWDialog* vtkKWDialog::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkKWDialog");
  if(ret)
    {
    return (vtkKWDialog*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkKWDialog;
}

int vtkKWDialogCommand(ClientData cd, Tcl_Interp *interp,
                             int argc, char *argv[]);

vtkKWDialog::vtkKWDialog()
{
  this->CommandFunction = vtkKWDialogCommand;
  this->Command = NULL;
  this->Done = 1;
}

vtkKWDialog::~vtkKWDialog()
{
  if (this->Command)
    {
    delete [] this->Command;
    }
}

int vtkKWDialog::Invoke()
{
  this->Done = 0;

  // map the window
  this->Script("wm deiconify %s",this->GetWidgetName());
  this->Script("focus %s",this->GetWidgetName());
  this->Script("grab %s",this->GetWidgetName());

  // do a grab
  // wait for the end
  while (!this->Done)
    {
    Tcl_DoOneEvent(0);    
    }
  this->Script("grab release %s",this->GetWidgetName());

  return (this->Done-1);
}

void vtkKWDialog::Display()
{
  this->Done = 0;

  // map the window
  this->Script("wm deiconify %s",this->GetWidgetName());
  this->Script("focus %s",this->GetWidgetName());
  this->Script("grab %s",this->GetWidgetName());
}

void vtkKWDialog::Cancel()
{
  this->Script("wm withdraw %s",this->GetWidgetName());
  this->Script("grab release %s",this->GetWidgetName());

  this->Done = 1;  
  if (this->Command && strlen(this->Command) > 0)
    {
    this->Script("eval %s",this->Command);
    }
}

void vtkKWDialog::OK()
{
  this->Script("wm withdraw %s",this->GetWidgetName());
  this->Script("grab release %s",this->GetWidgetName());
  this->Done = 2;  
  if (this->Command && strlen(this->Command) > 0)
    {
    this->Script("eval %s",this->Command);
    }
}

void vtkKWDialog::Create(vtkKWApplication *app, const char *args)
{
  const char *wname;

  // must set the application
  if (this->Application)
    {
    vtkErrorMacro("Dialog already created");
    return;
    }

  this->SetApplication(app);

  // create the top level
  wname = this->GetWidgetName();
  this->Script("toplevel %s %s",wname,args);
  this->Script("wm title %s \"Kitware Dialog\"",wname);
  this->Script("wm iconname %s \"Dialog\"",wname);
  this->Script("wm protocol %s WM_DELETE_WINDOW {%s Cancel}",
               wname, this->GetTclName());
  this->Script("wm withdraw %s",wname);
}

void vtkKWDialog::SetCommand(vtkKWObject* CalledObject, const char *CommandString)
{
  this->Command = this->CreateCommand(CalledObject, CommandString);
}

void vtkKWDialog::SetTitle( const char* title )
{
  this->Script("wm title %s \"%s\"", this->GetWidgetName(), 
	       title);
}

