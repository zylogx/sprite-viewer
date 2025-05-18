#include "sprite.h"
#include "assert.h"
#include <memory>

#define RAYGUI_IMPLEMENTATION
#include "raygui/src/raygui.h"

#undef RAYGUI_IMPLEMENTATION // Avoid including raygui implementation again
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "gui_window_file_dialog.h"

const int screenWidth = 1024;
const int screenHeight = 768;

int main()
{   
    InitWindow(screenWidth, screenHeight, "Sprite Viewer");
    SetTargetFPS(60);

    // Custom file dialog
    GuiWindowFileDialogState fileDialogState = InitGuiWindowFileDialog(GetWorkingDirectory());

    char fileNameToLoad[512] {0};
    bool warningMessage = false;

    Vector2 pos {40, 40};
    int frameCol = 8;
    int frameRow = 3;
    float frameFacing = 1.0f;

    float frameScale = 3.0f;
    float frameSpeed = 8.0f;

    int totalFrame = 6;
    const char* totalFrames {"0;1;2;3;4;5;6;7;8;9;10;11;12;13;14;15;16;17;18;19;20"};
    bool totalFrameDropdown = false;

    int selectedRow = 0;
    bool rowDropdown = false;
    const char* rowOptions = "Row 1;Row 2;Row 3";

    std::unique_ptr<Sprite> sprite {nullptr};

    float textureViewScale = 1.0f;
    
    while (!WindowShouldClose())
    {
        if (sprite != nullptr)
        {
            sprite->Update(pos, frameScale, frameSpeed, selectedRow, frameFacing, totalFrame);
        }

        if (fileDialogState.SelectFilePressed)
        {
            // Load file (if supported extension)
            if (IsFileExtension(fileDialogState.fileNameText, ".png"))
            {
                strcpy(fileNameToLoad, TextFormat("%s" PATH_SEPERATOR "%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
                
                sprite = std::make_unique<Sprite>(pos, fileNameToLoad, frameCol, frameRow, frameFacing);
            }
            else
            {
                warningMessage = true;
            }

            fileDialogState.SelectFilePressed = false;
        }

        BeginDrawing();
        DrawFPS(10, 10);
        ClearBackground(WHITE);

        if (sprite != nullptr)
        {
            sprite->Draw();
        }

        const int uiLeft = screenWidth - 250;
        GuiGroupBox((Rectangle){uiLeft, 20, 230, 180}, "Sprite Settings");

        GuiSliderBar(
            (Rectangle){uiLeft + 80, 50 + 20*0, 100, 15},
            "Frame Speed",
            TextFormat("%3.2f", frameSpeed),
            &frameSpeed,
            1.0f,
            20.0f
        );

        GuiSliderBar(
            (Rectangle){uiLeft + 80, 50 + 20*1, 100, 15},
            "Frame Scale",
            TextFormat("%1.2fx", frameScale),
            &frameScale,
            0.1f,
            10.0f
        );

        DrawText("Row", uiLeft + 55, 52 + 20*2, 9, BLACK);

        if (GuiDropdownBox(
                (Rectangle){ uiLeft + 80, 50 + 20*2, 100, 20 },
                rowOptions,
                &selectedRow,
                rowDropdown
            ))
        {
            rowDropdown = !rowDropdown;
        }

        DrawText("Total Frame", uiLeft + 15, 55 + 20*3, 9, BLACK);

        if (!rowDropdown)
        {
            if (GuiDropdownBox(
                    (Rectangle){uiLeft + 80, 55 + 20*3, 100, 15},
                    totalFrames,
                    &totalFrame,
                    totalFrameDropdown
                ))
            {
                totalFrameDropdown = !totalFrameDropdown;
            }
        }

        GuiGroupBox((Rectangle){uiLeft, 240, 230, 180}, "Texture View Settings");

        GuiSliderBar(
            (Rectangle){uiLeft + 80, 270 + 20*0, 100, 15},
            "Scale",
            TextFormat("%3.2f", textureViewScale),
            &textureViewScale,
            0.5f,
            2.0f
        );

        //----------------------------------------------------------------
        if (fileDialogState.windowActive)
        {
            GuiLock();
        }

        if (GuiButton((Rectangle){ 20, 35, 140, 30 }, GuiIconText(ICON_FILE_OPEN, "Load Sprite")))
        {
            fileDialogState.windowActive = true;
        }

        GuiUnlock();

        GuiWindowFileDialog(&fileDialogState);

        //----------------------------------------------------------------
        if (warningMessage)
        {
            int result = GuiMessageBox(
                (Rectangle){ screenWidth/2 - 100, screenHeight/2 - 100, 250, 100 },
                    "#191#Message Box", 
                    "The file should be a .png file.", 
                    "OK"
            );
            
            if (result >= 0)
            {
                warningMessage = false;
            }
        }

        if (sprite != nullptr)
        {
            const float textureScale = textureViewScale;

            const Vector2 texturePos {screenWidth - 570, screenHeight - 240};
            const Rectangle frameRec {sprite->GetFrameRec()};

            // Draw the full texture boundary
            DrawRectangleLines(
                texturePos.x,
                texturePos.y,
                sprite->GetTexture().width*textureScale,
                sprite->GetTexture().height*textureScale,
                LIME
            );

            // Scale frameRec position and size
            DrawRectangleLines(
                texturePos.x + frameRec.x*textureScale,
                texturePos.y + frameRec.y*textureScale,
                frameRec.width*textureScale,
                frameRec.height*textureScale,
                RED
            );

            // Draw the texture
            DrawTextureEx(sprite->GetTexture(), texturePos, 0.0f, textureScale, WHITE);
        }

        EndDrawing();
    }
    
    CloseWindow();
}