

#include "IntroduceChapterView.h"
#include <engine/BaseEngine.h>
#include <logic/DialogManager.h>

UI::IntroduceChapterView::IntroduceChapterView(Engine::BaseEngine& e)
    : View(e)
{
    m_pImageView = new ImageView(e);
    addChild(m_pImageView);
    m_pImageView->setAlignment(EAlign::A_Center);
    m_pImageView->setTranslation(Math::float2(0.25, 0.20)); //TODO: get this right
    m_pImageView->setRelativeSize(false);
    m_pImageView->setSize(Math::float2(4.5/8.0, 3.0/4.5));

    m_pTitleTextView = new TextView(e);
    addChild(m_pTitleTextView);
    m_pTitleTextView->setAlignment(EAlign::A_Center);
    m_pTitleTextView->setTranslation(Math::float2(0.5, 0.28));  //TODO: get this right
    m_pTitleTextView->setFont(DEFAULT_FONT_LARGE);

    m_pSubtitleTextView = new TextView(e);
    addChild(m_pSubtitleTextView);
    m_pSubtitleTextView->setAlignment(EAlign::A_Center);
    m_pSubtitleTextView->setTranslation(Math::float2(0.5, 0.78));   //TODO: get this right
    m_pSubtitleTextView->setFont(DEFAULT_FONT);

    initializeIntroduceChapterView();
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

void UI::IntroduceChapterView::initializeIntroduceChapterView()
{
    m_Title = "";
    m_Subtitle = "";
    m_TextureName = ""; //TODO: some default
    m_SoundName = "";
    m_WaitTime = 0;

    m_QueueStatus = QueueStatus::Empty;

    setHidden(true);

    m_pImageView->setHidden(true);
    m_pTitleTextView->setHidden(true);
    m_pSubtitleTextView->setHidden(true);
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
            Textures::TextureAllocator& alloc = m_Engine.getEngineTextureAlloc();
            Handle::TextureHandle image = alloc.loadTextureVDF(m_TextureName);
            if (image.isValid())
            {
                m_pImageView->setImage(image);
                m_pImageView->setHidden(false);
            }
        }

        //Setup Text
        {
            m_pTitleTextView->setText(m_Title);
            m_pTitleTextView->setHidden(false);
        }
        {
            m_pSubtitleTextView->setText(m_Subtitle);
            m_pTitleTextView->setHidden(false);
        }

        //Setup Sound

        //TODO: Pause Engine?

        m_QueueStatus = QueueStatus::Dequeued;

        setHidden(false);
    }

    View::update(dt, mstate, config);

    m_WaitTime -= dt;

    if ( m_WaitTime < 0 )
    {
        //TODO: unpause Engine?

        initializeIntroduceChapterView();        
    }
}

void UI::IntroduceChapterView::enqueueChapterIntroduction(std::string title, std::string subtitle, std::string texture_name, std::string sound_name, double wait_time)
{
    m_Title = title;
    m_Subtitle = subtitle;
    m_TextureName = texture_name;
    m_SoundName = sound_name;
    m_WaitTime = wait_time/1000;

    m_QueueStatus = QueueStatus::InQueue;
}
