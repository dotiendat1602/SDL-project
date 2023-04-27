#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <fstream>

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 627;
const string WINDOW_TITLE = "Game 2048";

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* gTexture = NULL;
Mix_Music* gMusic = NULL;
//Event handler
SDL_Event e;


void RandomValue();     // tạo giá trị ngẫu nhiên tại ô vuông (2)
void UpDate();
void LoadScore();
bool CheckEnd();
void EndGame();

void KEYUP();
void KEYDOWN();
void KEYLEFT();
void KEYRIGHT();

void NewGame();
void SaveGame();

bool init();
bool loadMedia();
void close();

SDL_Texture* loadTexture(SDL_Renderer* gRenderer, string path );

SDL_Rect GetRect(int value);  // lấy vị trí ảnh của giá trị value



class Square
{
private:
    int Value = 0;      //giá trị của ô
    SDL_Rect srcrect;   // vị trí ảnh của giá trị
    SDL_Rect dstrect;   // vị trí của ô trên window để in ảnh

public:
    SDL_Renderer* gRenderer;
    SDL_Texture* gTexture;

    void Set(int x, int y, SDL_Renderer* renderer, SDL_Texture* texture); // khởi tạo ô thay cho hàm khởi tạo
    void SetValue(int value); // cập nhật giá trị và ảnh trên render
    int GetValue() {
        return Value;   // trả về giá trị của ô cho mục đích xử lý
        }
};

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "linear" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}


		//Create window
		gWindow = SDL_CreateWindow( WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
				//Initialize Music
				if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
				{
					printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
					success = false;
				}

				SDL_RenderSetLogicalSize(gRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load PNG texture
	gTexture = loadTexture(gRenderer, "2048.bmp" );
	if( gTexture == NULL )
	{
		printf( "Failed to load texture image!\n" );
		success = false;
	}
	gMusic = Mix_LoadMUS("MY TALKING TOM - 2048 SOUNDTRACK OST.mp3");
	if( gMusic == NULL )
	{
		printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	return success;
}

void close()
{
	//Free loaded image
	SDL_DestroyTexture( gTexture );
	gTexture = NULL;

	//Destroy window
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;
	Mix_FreeMusic( gMusic );
	gMusic = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
	Mix_Quit();
}

SDL_Texture* loadTexture(SDL_Renderer* gRenderer, string path )
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return newTexture;
}

void Square::Set(int x, int y, SDL_Renderer* renderer, SDL_Texture* texture) {
    gRenderer = renderer;
    gTexture = texture;

    srcrect = GetRect(Value);

    dstrect.x = 18 + x * 152;
    dstrect.y = 18 + y * 152;
    dstrect.w = 135;
    dstrect.h = 135;

    SDL_RenderCopy(gRenderer,gTexture,&srcrect,&dstrect);
}

void Square::SetValue(int value) {
    if (Value == value) return ;
    Value = value;
    srcrect = GetRect(Value);
    SDL_RenderCopy(gRenderer,gTexture,&srcrect,&dstrect);
}

SDL_Rect GetRect(int value) {
    SDL_Rect rect;
    rect.w = 135;
    rect.h = 135;
    switch (value) {
    case 0: rect.x = 18; rect.y = 18;
        break;
    case 2: rect.x = 170; rect.y = 18;
        break;
    case 4: rect.x = 322; rect.y = 18;
        break;
    case 8: rect.x = 474; rect.y = 18;
        break;
    case 16: rect.x = 18; rect.y = 170;
        break;
    case 32: rect.x = 170; rect.y = 170;
        break;
    case 64: rect.x = 322; rect.y = 170;
        break;
    case 128: rect.x = 474; rect.y = 170;
        break;
    case 256: rect.x = 18; rect.y = 322;
        break;
    case 512: rect.x = 170; rect.y = 322;
        break;
    case 1024: rect.x = 322; rect.y = 322;
        break;
    case 2048: rect.x = 474; rect.y = 322;
        break;
    }
    return rect;
}

    Square** square;    // lưu giá trị của 16 ô square[x][y]
    int** Value;    // lưu giá trị tạm thời của square cho mục đích xử lý

    int Score = 0;


    /*
    khởi tạo game bằng SetData();
    vòng while vô hạn của game
        {
            đợi phím nhấn thì chạy Update() xong kiểm tra kết thúc game bằng CheckEnd()
            đợi 100ms = 0.1s rồi kiểm tra phím nhấn lần nữa
        }
    xong game thì chạy EndGame() xử lý kết quả thoát game
*/


int main(int argc, char* argv[]) {
    srand(time(NULL));

    //Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{

			//Main loop flag
			bool quit = false;
			SDL_Rect rect;
            rect.x = 0;
            rect.y = 0;
            rect.w = SCREEN_WIDTH;
            rect.h = SCREEN_HEIGHT;
            SDL_RenderCopy(gRenderer,gTexture,&rect,&rect);

            square = new Square* [4];        // cấp phát và khởi tạo cho square, value
            Value = new int* [4];
            for (int x = 0; x < 4; x++) {
                square[x] = new Square[4];
                for (int y = 0; y < 4; y++) square[x][y].Set(x,y,gRenderer,gTexture);
                Value[x] = new int[4];
            }

            //load dữ liệu lần chơi trc
            int val = 0;
            ifstream ifile("Score.txt");
            for (int y = 0; y < 4; y++)
                for (int x = 0; x < 4; x++) {
                    ifile >> val;
                    square[x][y].SetValue(val);
                }
            ifile.close();

            //cập nhật lần đầu cho game
            LoadScore();
            SDL_RenderPresent(gRenderer);

			//While application is running
			while( !quit )
			{
                if (SDL_PollEvent( &e ) != 0 && (e.type == SDL_KEYDOWN || e.type == SDL_QUIT)) {
                    if (e.type == SDL_QUIT) {
                        SaveGame();
                        exit(1);
                    }
                    else if(e.type == SDL_KEYDOWN){
                    UpDate();
                    if ( !CheckEnd() ) quit = true;
                    }
                }
                SDL_Delay(100);

				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}

    EndGame();

	//Free resources and close SDL
	close();
    return 0;
}

void UpDate() {
    // sao lưu sqare vào value để xử lý
    for (int x=0;x<4;x++) {
        for (int y=0;y<4;y++) Value[x][y] = square[x][y].GetValue();
    }


    switch (e.key.keysym.sym) {
        case SDLK_F1: NewGame();
            return ;
        case SDLK_F2: SaveGame();
            return ;
        case SDLK_UP: KEYUP();
            break;
        case SDLK_DOWN: KEYDOWN();
            break;
        case SDLK_LEFT: KEYLEFT();
            break;
        case SDLK_RIGHT: KEYRIGHT();
            break;
        default: return ; // phím khác sẽ không chấp nhận
            break;
    }

    // kiểm tra phím hợp lệ (nghĩa là có sự thay đổi của Value so với square ban đầu
    bool check = false;
    for (int x=0;x<4;x++) {
        for (int y=0;y<4;y++) {
            if (Value[x][y] != square[x][y].GetValue()){
                check = true;
                square[x][y].SetValue(Value[x][y]);
            }
        }
    }

    // nếu hợp lệ chạy cập nhật

    if (check) {
        LoadScore();
        SDL_RenderPresent(gRenderer);
        SDL_Delay(50);

        RandomValue();
        SDL_RenderPresent(gRenderer);
    }
}

void LoadScore() {
    // gán Score là giá trị lớn nhất của square::Value
    for (int x=0;x<4;x++)
    for (int y=0;y<4;y++) if (square[x][y].GetValue() > Score) Score = square[x][y].GetValue();

    // vẽ lại Score lên renderer
    static SDL_Rect srcrect;
    static SDL_Rect dstrect;
    srcrect = GetRect(Score);
    dstrect.x = 745;
    dstrect.y = 470;
    dstrect.w = 135;
    dstrect.h = 135;
    SDL_RenderCopy(gRenderer,gTexture,&srcrect,&dstrect);
}

bool CheckEnd() {
    // kết thúc false khi lên 2048
    for (int x=0;x<4;x++)
        for (int y=0;y<4;y++)
            if (square[x][y].GetValue()==2048) return false;

    // chưa lên 2048 còn ô rỗng tiếp tục true
    for (int x=0;x<4;x++)
        for (int y=0;y<4;y++)
            if (square[x][y].GetValue()==0) return true;

    // chưa lên 2048, kín 16 ô
    for (int x=0;x<4;x++) {
        if (square[x][0].GetValue() == square[x][1].GetValue()) return true;
        if (square[x][1].GetValue() == square[x][2].GetValue()) return true;
        if (square[x][2].GetValue() == square[x][3].GetValue()) return true;
    }
    for (int y=0;y<4;y++) {
        if (square[0][y].GetValue() == square[1][y].GetValue()) return true;
        if (square[1][y].GetValue() == square[2][y].GetValue()) return true;
        if (square[2][y].GetValue() == square[3][y].GetValue()) return true;
    }
    return false;   // trả về false thua cuộc nếu ko có ô nào tiếp tục được
}

void EndGame() {
    SDL_Rect rect,src,dst;

    // xóa 16 ô bằng hình vuông để  viết thông báo
    rect.x = 0; rect.y = 0; rect.w = 627; rect.h = 627;
    SDL_SetRenderDrawColor(gRenderer,255,255,255,255);
    SDL_RenderFillRect(gRenderer,&rect);

    // tải thông báo theo kết quả Score
    src.x = 0; src.y = 628; src.w = 450; src.h = 100;
    dst.x = 75; dst.y = 250; dst.w = 450; dst.h = 100;
    if (Score != 2048) src.x = 451;
    SDL_RenderCopy(gRenderer,gTexture,&src,&dst);

    SDL_RenderPresent(gRenderer);    // update từ renderer lên window


    // đợi kết thúc
    while (true) {if (SDL_PollEvent(&e) != 0 && ( e.type == SDL_QUIT)) break;}
    SDL_Delay(2000);


    // giải phóng con trỏ,...
    for (int x=0;x<4;x++) {delete[] square[x]; delete[] Value[x];}
}

void RandomValue() {
    int rannumber = 0;

    // đếm số lượng ô rỗng
    for (int x=0;x<4;x++)
        for (int y=0;y<4;y++) if (square[x][y].GetValue() == 0) rannumber++;

    // xác suất ô rỗng được gán giá trị ngẫu nhiên Val
    rannumber = rand() % rannumber + 1;
    int Val = 2;
    if (Score >= 32 && rand()%1024 < Score) Val = 4;
    if (Score >= 512 && rand()%10240 < Score) Val = 8;

    // cập nhật Val cho ô rỗng đã chọn
    for (int x=0;x<4;x++)
        for (int y=0;y<4;y++) {
            if (square[x][y].GetValue() == 0) rannumber--;
            if (rannumber==0) {square[x][y].SetValue(Val);return ;}
        }
}


// KEY công thức chung
/*
    xử lý trên dữ liệu ảo Value[x][y]
    xử 4 dòng tương tự nhau nên for ( =0; <4; ++)
    sắp xếp dồn 4 ô lại
    ô cạnh nhau cùng giá trị -> gộp lại 1 ô nhân đôi 1 ô về 0
    sắp xếp lại lần nữa
*/

void KEYUP() {
    for (int x=0;x<4;x++) {
        for (int i=0;i<3;i++)
            if (Value[x][i]==0)
                for (int j=i+1;j<4;j++)
                    if (Value[x][j]!=0) {
                        Value[x][i] = Value[x][j];
                        Value[x][j] = 0;
                        break;
                    }
        for (int i=0;i<3;i++)
            if (Value[x][i]==Value[x][i+1]) {
                Value[x][i] = 2*Value[x][i];
                Value[x][i+1] = 0;
            }
        for (int i=0;i<3;i++)
            if (Value[x][i]==0)
                for (int j=i+1;j<4;j++)
                    if (Value[x][j]!=0) {
                        Value[x][i] = Value[x][j];
                        Value[x][j] = 0;
                        break;
                    }
    }
}
void KEYDOWN() {
    for (int x=0;x<4;x++) {
        for (int i=3;i>0;i--)
            if (Value[x][i]==0)
                for (int j=i-1;j>=0;j--)
                    if (Value[x][j]!=0) {
                        Value[x][i] = Value[x][j];
                        Value[x][j] = 0;
                        break;
                    }
        for (int i=3;i>0;i--)
            if (Value[x][i]==Value[x][i-1]) {
                Value[x][i] = 2*Value[x][i];
                Value[x][i-1] = 0;
            }
        for (int i=3;i>0;i--)
            if (Value[x][i]==0)
                for (int j=i-1;j>=0;j--)
                    if (Value[x][j]!=0) {
                        Value[x][i] = Value[x][j];
                        Value[x][j] = 0;
                        break;
                    }
    }
}
void KEYLEFT() {
    for (int y=0;y<4;y++) {
        for (int i=0;i<3;i++)
            if (Value[i][y]==0)
                for (int j=i+1;j<4;j++)
                    if (Value[j][y]!=0) {
                        Value[i][y] = Value[j][y];
                        Value[j][y] = 0;
                        break;
                    }
        for (int i=0;i<3;i++)
            if (Value[i][y]==Value[i+1][y]) {
                Value[i][y] = 2*Value[i][y];
                Value[i+1][y] = 0;
            }
        for (int i=0;i<3;i++)
            if (Value[i][y]==0)
                for (int j=i+1;j<4;j++)
                    if (Value[j][y]!=0) {
                        Value[i][y] = Value[j][y];
                        Value[j][y] = 0;
                        break;
                    }
    }
}
void KEYRIGHT() {
    for (int y=0;y<4;y++) {
        for (int i=3;i>0;i--)
            if (Value[i][y]==0)
                for (int j=i-1;j>=0;j--)
                    if (Value[j][y]!=0) {
                        Value[i][y] = Value[j][y];
                        Value[j][y] = 0;
                        break;
                    }
        for (int i=3;i>0;i--)
            if (Value[i][y]==Value[i-1][y]) {
                Value[i][y] = 2*Value[i][y];
                Value[i-1][y] = 0;
            }
        for (int i=3;i>0;i--)
            if (Value[i][y]==0)
                for (int j=i-1;j>=0;j--)
                    if (Value[j][y]!=0) {
                        Value[i][y] = Value[j][y];
                        Value[j][y] = 0;
                        break;
                    }
    }
}

void NewGame() {
    // làm mới Score và square::Value -> chạy ngẫu nhiên ô rỗng -> update lên window
    Score = 0;
    for (int y=0;y<4;y++)
        for (int x=0;x<4;x++) square[x][y].SetValue(0);
    RandomValue();
    LoadScore();
    SDL_RenderPresent(gRenderer);
}
void SaveGame() {
    // lưu giá trị toàn bộ 16 square::Value xuống file
    ofstream ofile("Score.txt");
    for (int y=0;y<4;y++) {
        for (int x=0;x<4;x++) ofile << square[x][y].GetValue() << " ";
        ofile << std::endl;
    }
    ofile.close();
}


