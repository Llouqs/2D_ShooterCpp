#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>  // Для std::shared_ptr

class UIManager {
private:
    std::shared_ptr<Text> HealthText;
    std::shared_ptr<Text> ScoreText;
    std::shared_ptr<Text> RecordText;
    Font MainFont;
    std::string MainFontPath;
    Color TextColor;

    static constexpr float HEALTH_POS_X = 30.f;
    static constexpr float HEALTH_POS_Y = 30.f;
    static constexpr float SCORE_POS_X = 30.f;
    static constexpr float SCORE_POS_Y = 60.f;
    static constexpr float RECORD_POS_X = 30.f;
    static constexpr float RECORD_POS_Y = 90.f;
    static constexpr int FONT_SIZE = 20;

public:
    UIManager(int health, int scores)
        : TextColor(Color::White), MainFontPath("fonts/Riviera.otf")
    {
        if (!MainFont.openFromFile(MainFontPath))
            std::cerr << "Do not found " << MainFontPath << " in 'fonts/'" << std::endl;
        HealthText = std::make_shared<Text>(MainFont);
        HealthText->setCharacterSize(FONT_SIZE);
        HealthText->setFillColor(TextColor);
        HealthText->setPosition({ HEALTH_POS_X, HEALTH_POS_Y });
        setHealthText(health);

        ScoreText = std::make_shared<Text>(MainFont);
        ScoreText->setCharacterSize(FONT_SIZE);
        ScoreText->setFillColor(TextColor);
        ScoreText->setPosition({ SCORE_POS_X, SCORE_POS_Y });
        setScoreText(scores);

        RecordText = std::make_shared<Text>(MainFont);
        RecordText->setCharacterSize(FONT_SIZE);
        RecordText->setFillColor(TextColor);
        RecordText->setPosition({ RECORD_POS_X, RECORD_POS_Y });
        setRecordText(0);
    }

    void setHealthText(int health) {
        if (health <= 50 && TextColor != Color::Red) {
            TextColor = Color::Red;
            HealthText->setFillColor(TextColor);
        }
        else if (health > 50 && TextColor == Color::Red) {
            TextColor = Color::White;
            HealthText->setFillColor(TextColor);
        }
        HealthText->setString("Health: " + std::to_string(health));
    }

    void setScoreText(int scores) {
        ScoreText->setString("Scores: " + std::to_string(scores));
    }

    void setRecordText(int scores) {
        RecordText->setString("Record: " + std::to_string(scores) + "                 PRESS SPACE TO RESTART");
    }

    const Text& getRecordText() const { return *RecordText; }
    const Text& getScoreText() const { return *ScoreText; }
    const Text& getHealthText() const { return *HealthText; }
};
