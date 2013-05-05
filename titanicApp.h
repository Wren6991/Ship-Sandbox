/***************************************************************
 * Name:      titanicApp.h
 * Purpose:   Defines Application Class
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2013-04-30
 * Copyright: Luke Wren (http://github.com/Wren6991)
 * License:
 **************************************************************/

#ifndef TITANICAPP_H
#define TITANICAPP_H

#include <wx/app.h>

class titanicApp : public wxApp
{
    public:
        virtual bool OnInit();
};

#endif // TITANICAPP_H
