/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef FRONTENDAPPLICATIONBASE_HPP
#define FRONTENDAPPLICATIONBASE_HPP

#include <mvp/MVPApplication.hpp>
#include <gui/model/Model.hpp>

class FrontendHeap;

class FrontendApplicationBase : public touchgfx::MVPApplication
{
public:
    FrontendApplicationBase(Model& m, FrontendHeap& heap);
    virtual ~FrontendApplicationBase() { }

    // MainScreen
    void gotoMainScreenScreenNoTransition();

    void gotoMainScreenScreenWipeTransitionWest();

    // LedControl
    void gotoLedControlScreenSlideTransitionEast();

    // TicTacToe
    void gotoTicTacToeScreenNoTransition();

    // BMP280
    void gotoBMP280ScreenNoTransition();

    // Apk2
    void gotoApk2ScreenNoTransition();

    // MP3Player
    void gotoMP3PlayerScreenNoTransition();

    // AnalogowyZegarek
    void gotoAnalogowyZegarekScreenNoTransition();

protected:
    touchgfx::Callback<FrontendApplicationBase> transitionCallback;
    FrontendHeap& frontendHeap;
    Model& model;

    // MainScreen
    void gotoMainScreenScreenNoTransitionImpl();

    void gotoMainScreenScreenWipeTransitionWestImpl();

    // LedControl
    void gotoLedControlScreenSlideTransitionEastImpl();

    // TicTacToe
    void gotoTicTacToeScreenNoTransitionImpl();

    // BMP280
    void gotoBMP280ScreenNoTransitionImpl();

    // Apk2
    void gotoApk2ScreenNoTransitionImpl();

    // MP3Player
    void gotoMP3PlayerScreenNoTransitionImpl();

    // AnalogowyZegarek
    void gotoAnalogowyZegarekScreenNoTransitionImpl();
};

#endif // FRONTENDAPPLICATIONBASE_HPP
