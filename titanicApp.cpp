/***************************************************************
 * Name:      titanicApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2013-04-30
 * Copyright: Luke Wren (http://github.com/Wren6991)
 * License:
 **************************************************************/

#include "titanicApp.h"

//(*AppHeaders
#include "titanicMain.h"
#include <wx/image.h>
//*)

//IMPLEMENT_APP(titanicApp);

#include <iostream>

bool titanicApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	titanicFrame* Frame = new titanicFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}


int main()
{
    std::cout << "Hello from main!\n";
    char c;
    std::cin >> c;
}
