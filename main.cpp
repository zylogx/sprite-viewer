#include "sprite.h"
#include "assert.h"

#include <string>
#include <memory>

#define RAYGUI_IMPLEMENTATION
#include "raygui/src/raygui.h"

#undef RAYGUI_IMPLEMENTATION // Avoid including raygui implementation again
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "gui_window_file_dialog.h"

const int screenWidth = 1024;
const int screenHeight = 768;

#define GRID_SIZE 72

static void DrawGrid(int x, int y, int width, int height)
{
    int cellWidth = width / GRID_SIZE;
    int cellHeight = height / GRID_SIZE;

    for (int row = 0; row < GRID_SIZE; row++)
    {
        for (int col = 0; col < GRID_SIZE; col++)
        {
            Color color = ((row + col) % 2 == 0) ? LIGHTGRAY : DARKGRAY;
            DrawRectangle(x + col * cellWidth, y + row * cellHeight, cellWidth, cellHeight, color);
        }
    }
}

static void DrawTexturePreview(const Vector2& pos, const Sprite* sprite, int width, int height)
{
    const int textureWidth = width;
    const int textureHeight = height;

    // Draw the full texture boundary
    DrawRectangleLinesEx((Rectangle){pos.x, pos.y, static_cast<float>(textureWidth), static_cast<float>(textureHeight)}, 2.5f, GRAY);

    if (sprite != nullptr)
    {
        const Texture2D texture {sprite->GetTexture()};

        DrawTexturePro(
            texture,
            (Rectangle){0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)}, // Use full texture
            (Rectangle){pos.x, pos.y, static_cast<float>(textureWidth), static_cast<float>(textureHeight)},
            (Vector2){0, 0}, // Origin at top-left
            0.0f,            // No rotation
            WHITE            // No tint
        );

        const Rectangle frameRec = sprite->GetFrameRec();

        // Compute scaling factors from original texture to fixed size
        const float scaleX = textureWidth/static_cast<float>(texture.width);
        const float scaleY = textureHeight/static_cast<float>(texture.height);

        // Draw scaled frameRec inside the scaled texture
        DrawRectangleLines(
            pos.x + frameRec.x*scaleX,
            pos.y + frameRec.y*scaleY,
            frameRec.width*scaleX,
            frameRec.height*scaleY,
            RED
        );
    }
}

int main()
{   
    InitWindow(screenWidth, screenHeight, "Sprite Viewer");
    SetTargetFPS(60);

    // Custom file dialog
    GuiWindowFileDialogState fileDialogState = InitGuiWindowFileDialog(GetWorkingDirectory());

    char fileNameToLoad[512] {0};
    bool warningMessage = false;

    Vector2 pos {50, 100};

    int frameCol = 10;
    bool frameColDropdown = false;
    const char* frameColOptions {"0;1;2;3;4;5;6;7;8;9;10;11;12;13;14;15;16;17;18;19;20"};

    int frameRow = 6;
    bool frameRowDropdown = false;
    const char* frameRowOptions {"0;1;2;3;4;5;6;7;8;9;10;11;12;13;14;15;16;17;18;19;20"};

    float frameFacing = 1.0f;

    float frameScale = 3.0f;
    float frameSpeed = 8.0f;

    int totalFrames = 10;
    bool totalFramesDropdown = false;
    const char* charTotalFrames {"0;1;2;3;4;5;6;7;8;9;10;11;12;13;14;15;16;17;18;19;20"};

    int selectedRow = 0;
    bool rowDropdown = false;
    const char* rowOptions {"Row 1;Row 2;Row 3;Row 4;Row 5;Row 6;Row 7;Row 8"};

    int selectedAdvanceMode = 0;
    bool advanceMode = false;
    const char* advanceModeOptions {"False;True"};

    std::unique_ptr<Sprite> sprite {nullptr};

    bool hasAdvancedRow = false;

    unsigned int currentTime = 0;
    
    while (!WindowShouldClose())
    {
        currentTime = (unsigned int)GetTime();

        if (sprite != nullptr)
        {
            if ((sprite->GetCurrentFrame() + 1) >= totalFrames && !hasAdvancedRow)
            {
                selectedRow = selectedRow + 1;
                hasAdvancedRow = true;
            }
            else if (sprite->GetCurrentFrame() < totalFrames && selectedRow == frameRow)
            {
                selectedRow = 0;
                hasAdvancedRow = false;
            }
            else if (sprite->GetCurrentFrame() < totalFrames)
            {
                hasAdvancedRow = false;
            }

            sprite->Update(pos, frameScale, frameSpeed, selectedRow, frameFacing, totalFrames, static_cast<bool>(selectedAdvanceMode));
        }

        if (fileDialogState.SelectFilePressed)
        {
            // Load file (if supported extension)
            if (IsFileExtension(fileDialogState.fileNameText, ".png"))
            {
                strcpy(fileNameToLoad, TextFormat("%s" PATH_SEPERATOR "%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
                
                sprite.reset();
                sprite = std::make_unique<Sprite>(pos, fileNameToLoad, frameCol, frameRow, frameFacing);
            }
            else
            {
                warningMessage = true;
            }

            fileDialogState.SelectFilePressed = false;
        }

        BeginDrawing();
        ClearBackground(WHITE);
        DrawFPS(10, 10);
        DrawText("Current Time: ", 460, 80, 18, BLACK);
        DrawText(std::to_string(currentTime).c_str(), 580, 80, 18, BLACK);
        const Vector2 texturePos {screenWidth - 565, screenHeight - 350};
        DrawTexturePreview(texturePos, sprite.get(), 560, 340);

        int gridWidth = 480;
        int gridHeight = 480;

        DrawGrid(20, 70, gridWidth, gridHeight);

        if (sprite != nullptr)
        {
            sprite->Draw();
        }

        const int uiLeft = screenWidth - 250;
        GuiGroupBox((Rectangle){uiLeft, 70, 242, 340}, "Sprite Settings");

        GuiSliderBar(
            (Rectangle){uiLeft + 84, 80 + 20*0, 100, 15},
            "Frame Speed",
            TextFormat("%3.2f", frameSpeed),
            &frameSpeed,
            1.0f,
            20.0f
        );

        GuiSliderBar(
            (Rectangle){uiLeft + 84, 80 + 20*1, 100, 15},
            "Frame Scale",
            TextFormat("%1.2fx", frameScale),
            &frameScale,
            1.0f,
            6.0f
        );

        // -------------------------------
        // Dropdown ----------------------
        // -------------------------------

        DrawText("Advance Mode", uiLeft + 10, 85 + 20*6, 9, BLACK);

        if (GuiDropdownBox(
                (Rectangle){uiLeft + 84, 85 + 20*6, 100, 15},
                advanceModeOptions,
                &selectedAdvanceMode,
                advanceMode
            ))
        {
            advanceMode = !advanceMode;
        }

        DrawText("Columns", uiLeft + 10, 85 + 20*5, 9, BLACK);

        if (GuiDropdownBox(
                (Rectangle){uiLeft + 84, 85 + 20*5, 100, 15},
                frameColOptions,
                &frameCol,
                frameColDropdown
            ))
        {
            sprite.reset();
            frameColDropdown = !frameColDropdown;
        }

        DrawText("Rows", uiLeft + 10, 85 + 20*4, 9, BLACK);

        if (GuiDropdownBox(
                (Rectangle){uiLeft + 84, 85 + 20*4, 100, 15},
                frameRowOptions,
                &frameRow,
                frameRowDropdown
            ) && !frameColDropdown)
        {
            sprite.reset();
            frameRowDropdown = !frameRowDropdown;
        }

        DrawText("Total Frames", uiLeft + 10, 85 + 20*3, 9, BLACK);

        if (GuiDropdownBox(
                (Rectangle){uiLeft + 84, 85 + 20*3, 100, 15},
                charTotalFrames,
                &totalFrames,
                totalFramesDropdown
            ) && !frameRowDropdown)
        {
            totalFramesDropdown = !totalFramesDropdown;
        }

        DrawText("Row", uiLeft + 55, 82 + 20*2, 9, BLACK);

        if (GuiDropdownBox(
                (Rectangle){ uiLeft + 84, 80 + 20*2, 100, 20 },
                rowOptions,
                &selectedRow,
                rowDropdown
            ))
        {
            rowDropdown = !rowDropdown;
        }

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

        EndDrawing();
    }
    
    CloseWindow();
}