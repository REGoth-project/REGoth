//
// Created by markusobi on 19.05.17.
//

#include "LoadingScreen.h"
#include <engine/BaseEngine.h>
#include <render/RenderSystem.h>
#include "BarView.h"
#include "TextView.h"

/**
 * To hide long lasting operations where the bar isn't updated, it will move
 * slightly on its own inside the active section.
 * This controls how many section-intern percentage points should be added per second.
 */
const float FAKE_PROGRESS_PERCENT_PER_SECOND = 10.0f;
const float PROGRESS_BAR_SOFT_SPEED = 12.0f;

UI::LoadingScreen::LoadingScreen(Engine::BaseEngine &e) : ImageView(e)
{
    Textures::TextureAllocator& alloc = m_Engine.getEngineTextureAlloc();
    reset();
    m_RelativeSize = false;

    m_pProgressBar = new BarView(e);
    addChild(m_pProgressBar);

    m_pInfo = new TextView(e);
    addChild(m_pInfo);

    Handle::TextureHandle hBarBack = alloc.loadTextureVDF("PROGRESS.TGA");
    Handle::TextureHandle hBarFront = alloc.loadTextureVDF("PROGRESS_BAR.TGA");

    m_pProgressBar->setBackgroundImage(hBarBack);
    m_pProgressBar->setBarImage(hBarFront);

    m_pProgressBar->setAlignment(EAlign::A_Center);
    m_pProgressBar->setTranslation(Math::float2(0.5,0.75));
    m_pProgressBar->setSize(Math::float2(0.8f, 0.6f));

    // G2s image is a little bit different and needs some tweaking
    if(e.getBasicGameType() == Daedalus::GameType::GT_Gothic2)
    {
        m_pProgressBar->setInnerOffset(UI::INNER_OFFSET_G2_PROGRESS);
    }

    m_pInfo->setAlignment(EAlign::A_Center);
    m_pInfo->setTranslation(Math::float2(0.5,0.78));
    m_pInfo->setFont(DEFAULT_FONT);

    m_VisibleProgress = 0;
    m_SectionStart = 0;
    m_SectionEnd = 0;
    m_SectionInfo = "";
    m_SectionProgress = 0.0f;
}

UI::LoadingScreen::~LoadingScreen()
{
    removeChild(m_pProgressBar);
    delete m_pProgressBar;

    removeChild(m_pInfo);
    delete m_pInfo;
}

void UI::LoadingScreen::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    int start, end;
    float p;
    std::string info;

    // Get section params - threadsave
    getSection(start, end, p, info);

    // Add a slight amount so that the bar doesn't appear to be stuck
    p = std::min(p + (float)dt * FAKE_PROGRESS_PERCENT_PER_SECOND, 100.0f);
    setSectionProgress(p);

    // Convert section progress to global progress
    float value = bx::flerp(start, end, p / 100.0f);
    m_VisibleProgress = bx::flerp(m_VisibleProgress, value, (float)dt * PROGRESS_BAR_SOFT_SPEED);
    m_VisibleProgress = std::min(value, m_VisibleProgress); // Faked value should never overshoot the real one

    m_pProgressBar->setValue(m_VisibleProgress / 100.0f);
    m_pInfo->setText(info);

    ImageView::update(dt, mstate, config);
}

void UI::LoadingScreen::startSection(int p1, int p2, const std::string& message)
{
    std::lock_guard<std::mutex> guard(m_SectionLock);

    // Complete restart?
    if(p1 == 0)
        m_VisibleProgress = 0;

    m_SectionStart = p1;
    m_SectionEnd = p2;
    m_SectionInfo = message;
    m_SectionProgress = 0.0f;
}

void UI::LoadingScreen::setSectionProgress(float p)
{
    std::lock_guard<std::mutex> guard(m_SectionLock);

    // Don't allow going back due to the fake-progress
    if(m_SectionProgress < p)
        m_SectionProgress = p;
}

void UI::LoadingScreen::getSection(int& outStart, int& outEnd, float& outProgress, std::string& outInfo)
{
    std::lock_guard<std::mutex>  guard(m_SectionLock);

    outStart = m_SectionStart;
    outEnd = m_SectionEnd;
    outProgress = m_SectionProgress;
    outInfo = m_SectionInfo;
}

void UI::LoadingScreen::setImageFromFile(const std::string& imageName)
{
    Textures::TextureAllocator& alloc = m_Engine.getEngineTextureAlloc();
    Handle::TextureHandle bgr = alloc.loadTextureVDF(imageName);
    if (bgr.isValid())
        setImage(bgr);
}

void UI::LoadingScreen::reset(const std::string& imageName)
{
    setImageFromFile(imageName);
    startSection(0, 100, "");
    setSectionProgress(0);
}
