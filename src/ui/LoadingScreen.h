//
// Created by markusobi on 19.05.17.
//
#pragma once

#include <mutex>
#include "ImageView.h"

namespace UI
{
    class BarView;
    class TextView;
    class LoadingScreen : public ImageView
    {
    public:
        LoadingScreen(Engine::BaseEngine& e);
        ~LoadingScreen();

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config) override;

        /**
         * Sets the currently processed section on the loading bar. Each section can then
         * run from 0 to 100% individually.
         *
         * This Method is thread-save.
         *
         * @param p1 Percentage [0..100] for the starting point of the section
         * @param p2 Percentage [0..100] for the ending point of the section
         * @param message Optional message displayed with the section
         */
        void startSection(int p1, int p2, const std::string& message = "");

        /**
         * Sets the progress of the currently set section. For example, if the current section
         * starts at 10% and ends at 20%, setting this to to 50% will move the loading bar
         * to 15%.
         *
         * This Method is thread-save.
         *
         * @param p Percentage done of the current section [0..100]
         */
        void setSectionProgress(float p);

        /**
         * @param sets image to the image with the given filename
         */
        void setImageFromFile(const std::string& imageName = "LOADING.TGA");
    protected:

        /**
         * Thread-save access to the sections.
         */
        void getSection(int& outStart, int& outEnd, float& outProgress, std::string& outInfo);

        /**
         * Bar that shows the load-progress
         */
        BarView* m_pProgressBar;
        TextView* m_pInfo;

        /**
         * Information about the currently active section
         */
        int m_SectionStart = 0;
        int m_SectionEnd = 100;
        float m_SectionProgress = 0;
        std::string m_SectionInfo;

        /**
         * Global Progress in the range of [0..100], visible to the user and softened
         */
        float m_VisibleProgress;

        /**
         * Lock for updating the sections
         */
        std::mutex m_SectionLock;
    };
}

