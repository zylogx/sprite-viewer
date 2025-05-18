#pragma once

#include "raylib.h"

class Sprite
{
private:
    Vector2 position;
    Rectangle frameRec;
    Texture2D spriteSheet;

    int frameWidth;
    int frameHeight;
    int currentFrame;
    int frameCounter;
    float frameSpeed;
    float frameScale;
    float frameFacing;

    int frameColumns;
    int frameRows;

public:
    Sprite(Vector2 position_, const char* spriteSheetPath_, int frameColumns_, int frameRows_, float frameFacing_)
    {
        spriteSheet = LoadTexture(spriteSheetPath_);
        position = position_;

        frameColumns = frameColumns_;
        frameRows = frameRows_;

        frameWidth = spriteSheet.width/frameColumns_;
        frameHeight = spriteSheet.height/frameRows_;

        frameRec = {
            0, 0,
            static_cast<float>(frameWidth),
            static_cast<float>(frameHeight)
        };

        currentFrame = 0;
        frameCounter = 0;
        frameSpeed = 8.0f;
        frameScale = 2.0f;
        frameFacing = frameFacing_;
    }

    ~Sprite()
    {
        UnloadTexture(spriteSheet);
    }

    Texture2D GetTexture() const
    {
        return spriteSheet;
    }

    Rectangle GetFrameRec() const
    {
        return frameRec;
    }

    void Reset(float frameScale_ = 1.0f)
    {
        currentFrame = 0;
        frameCounter = 0;
        frameSpeed = 8.0f;
        frameScale = frameScale_;
    }

    void Update(Vector2 position_, float frameScale_, float frameSpeed_, int selectedRow_, float frameFacing_, int totalFrames_ = 0)
    {
        const int framesPerRow = spriteSheet.width/frameWidth*totalFrames_/frameColumns;

        frameScale = frameScale_;
        frameSpeed = frameSpeed_;
        frameFacing = frameFacing_;
        position = position_;

        frameCounter++;
        if (frameCounter >= (GetFPS()/frameSpeed))
        {
            currentFrame++;
            if (currentFrame >= framesPerRow) currentFrame = 0;

            const int row = selectedRow_;
            const int col = currentFrame;

            frameRec.x = col*frameWidth;
            frameRec.y = row*frameHeight;

            frameCounter = 0;
        }
    }

    void Draw() const
    {
        const Rectangle source{
            frameRec.x, frameRec.y,
            frameRec.width*frameFacing,
            frameRec.height
        };

        DrawTexturePro(
            spriteSheet,
            source,
            Rectangle{
            position.x, position.y,
                frameRec.width*frameScale,
                frameRec.height*frameScale
            },
            Vector2{0, 0}, 0.0f,
            WHITE
        );
    }
};