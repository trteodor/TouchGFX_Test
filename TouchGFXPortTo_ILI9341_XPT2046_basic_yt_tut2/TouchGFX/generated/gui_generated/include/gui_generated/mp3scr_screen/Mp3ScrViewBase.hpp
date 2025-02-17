/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef MP3SCRVIEWBASE_HPP
#define MP3SCRVIEWBASE_HPP

#include <gui/common/FrontendApplication.hpp>
#include <mvp/View.hpp>
#include <gui/mp3scr_screen/Mp3ScrPresenter.hpp>
#include <touchgfx/widgets/Box.hpp>

class Mp3ScrViewBase : public touchgfx::View<Mp3ScrPresenter>
{
public:
    Mp3ScrViewBase();
    virtual ~Mp3ScrViewBase() {}
    virtual void setupScreen();

protected:
    FrontendApplication& application() {
        return *static_cast<FrontendApplication*>(touchgfx::Application::getInstance());
    }
    touchgfx::Box __background;

private:

};

#endif // MP3SCRVIEWBASE_HPP
