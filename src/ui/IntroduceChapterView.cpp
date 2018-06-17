

#include "IntroduceChapterView.h"
#include <engine/BaseEngine.h>
#include <logic/DialogManager.h>
#include <audio/AudioWorld.h>

UI::IntroduceChapterView::IntroduceChapterView(Engine::BaseEngine& e)
    : View(e)
{
    m_pImageView = new ImageView(e);
    addChild(m_pImageView);
    m_pImageView->setRelativeSize(false);
    m_pImageView->setSize(Math::float2(1.0f, 1.0f));
    m_pImageView->setTranslation(Math::float2(.0f, .0f));

    m_pTitleTextView = new TextView(e);
    addChild(m_pTitleTextView);
    m_pTitleTextView->setFont(DEFAULT_FONT_LARGE);
    m_pTitleTextView->setAlignment(A_TopCenter);
    m_pTitleTextView->setTranslation(Math::float2(.5f, .085f));

    m_pSubtitleTextView = new TextView(e);
    addChild(m_pSubtitleTextView);
    m_pSubtitleTextView->setFont(DEFAULT_FONT);
    m_pSubtitleTextView->setAlignment(A_TopCenter);
    m_pSubtitleTextView->setTranslation(Math::float2(.5f, .885f));

    m_QueueStatus = QueueStatus::Empty;
}

UI::IntroduceChapterView::~IntroduceChapterView()
{
    removeChild(m_pImageView);
    delete m_pImageView;

    removeChild(m_pTitleTextView);
    delete m_pTitleTextView;

    removeChild(m_pSubtitleTextView);
    delete m_pSubtitleTextView;
}

void UI::IntroduceChapterView::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    if ( m_QueueStatus == QueueStatus::Empty || /* Dialog in progess */ m_Engine.getMainWorld().get().getDialogManager().isDialogActive() )
        return;

    //Dequeue
    if ( m_QueueStatus == QueueStatus::InQueue )
    {
        //Setup ImageView picture
        {
            m_pImageView->setImage(m_Texture);
        }

        //Setup Text
        {
            m_pTitleTextView->setText(m_Title);

            m_pSubtitleTextView->setText(m_Subtitle);
        }

        //Setup Sound
        World::AudioWorld& audioWorld = m_Engine.getMainWorld().get().getAudioWorld();
        {
            audioWorld.playSound(m_Sound);
        }

        // Pause Engine, but continue Sound
        m_Engine.setPaused(true);
        audioWorld.continueSounds(); //FIXME: might need to use OpenAL or AudioRewrite

        m_QueueStatus = QueueStatus::Dequeued;
        setHidden(false);
    }

    // Size tied to windowsize and default size of 640x480
    Math::float2 fac = Math::float2(640.0f / config.state.viewWidth, 480.0f / config.state.viewHeight);
    Math::float2 size = Math::float2(1.0f * fac.x, 1.0f * fac.y);
    setSize(size);

    // Center the view
    Math::float2 c = Math::float2(0.5f, 0.5f) - size * 0.5f;
    setTranslation(c);

    View::update(dt, mstate, config);

    m_WaitTime -= dt;

    if ( m_WaitTime < 0 )
    {
        m_Engine.setPaused(false);

        m_QueueStatus = QueueStatus::Empty;
        setHidden(true);
    }
}

void UI::IntroduceChapterView::enqueueChapterIntroduction(std::string title, std::string subtitle, std::string texture_name, std::string sound_name, double wait_time)
{
    m_Title = title;
    m_Subtitle = subtitle;

    Textures::TextureAllocator& alloc = m_Engine.getEngineTextureAlloc();
    m_Texture = alloc.loadTextureVDF(texture_name);

    m_Sound = m_Engine.getMainWorld().get().getAudioWorld().loadAudioVDF(sound_name);

    m_WaitTime = wait_time/1000;

    if ( m_Texture.isValid() && m_Sound.isValid() && m_WaitTime>0 )
        m_QueueStatus = QueueStatus::InQueue;
}

void UI::IntroduceChapterView::close()
{
    m_WaitTime = 0;
}