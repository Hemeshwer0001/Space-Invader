
#include <iostream>
#include <random> // to provide random coordinates to aliens
#include <fstream> // to store data in text files
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
using namespace std;

struct Missiles{
    sf::Sprite setSprite;
    string state = "rest"; // initially at rest
};

void moveShip(sf::Sprite& ship, float moveX, float deltaTime, bool Over){
    // here we will make click listener for the spaceship
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && Over == false){
        ship.move(-moveX*deltaTime, 0);
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && Over == false){
        ship.move(moveX*deltaTime, 0);
    }

    if(ship.getPosition().x+64 >= 800){
        ship.setPosition(736, 536);
    }
    else if(ship.getPosition().x <= 0){
        ship.setPosition(0, 536);
    }
}

void loadAliens(vector<sf::Sprite>&aliens, sf::Sprite& alien, int& totalAliens){
    // we will use <random> header file to get random coordinates for the aliens
    random_device rd;
    mt19937 gen(rd());
    
    for(int i = 0; i<totalAliens; i++){
        uniform_int_distribution<> X(0, 730); // range [0, 736]
        uniform_int_distribution<> Y(0, 200); // range [0, 200]
        int AlienX = X(gen); // will help us get random values
        int AlienY = Y(gen);
        alien.setPosition(AlienX, AlienY);
        aliens.push_back(alien);
    }
}

void displayAliens(vector<sf::Sprite>& aliens, sf::RenderWindow& window){
    for(int i = 0; i<aliens.size(); i++){
        window.draw(aliens[i]);
    }
}

void setMovement(vector<sf::Sprite>& aliens, vector<sf::Vector2f>& AlienMovement, int& totalAliens){
    for(int i = 0; i<totalAliens; i++){ // setting movements for all aliens
        AlienMovement.push_back({700.0, 75.0});
    }
}

void moveAliens(vector<sf::Sprite>& aliens, vector<sf::Vector2f>& AlienMovement, float deltaTime, bool& over){
    // moving aliens
    for(int i = 0; i<aliens.size(); i++){
        if(i%2 == 1) AlienMovement[i].y = 0;
        aliens[i].move(AlienMovement[i]*deltaTime);
    }

    // keeping the alien in bounds
    for(int i = 0; i<aliens.size(); i++){
        if(aliens[i].getPosition().x+64 >= 800){
            aliens[i].setPosition(736, aliens[i].getPosition().y+40);
            AlienMovement[i].x *= -1;
        }
        else if(aliens[i].getPosition().x <= 0){
            aliens[i].setPosition(0, aliens[i].getPosition().y+40);
            AlienMovement[i].x *= -1;
        }
        else if(aliens[i].getPosition().y+64 >= 566){
            over = true;
            for(int i = 0; i<aliens.size(); i++){
                AlienMovement[i].x = 0;
                AlienMovement[i].y = 0;
            }
            break;
        }
    }
}

void displayMissiles(vector<Missiles>& missiles, sf::RenderWindow& window){
    for(int i = 0; i<missiles.size(); i++){
        window.draw(missiles[i].setSprite);
    }
}

bool AlienMissileCollision(sf::Sprite& missile, vector<sf::Sprite>& aliens, string& state, vector<int>& Hp, sf::Sound& missileCollision, string& musicPreference){ // it wil tell us if the missile has collided with any alien
    for(int i = 0; i<aliens.size(); i++){
        if(missile.getGlobalBounds().intersects(aliens[i].getGlobalBounds())){
            if(musicPreference == "On"){
                missileCollision.setVolume(30);
                missileCollision.play();
            }
            missile.setPosition(-100, -100); // taking misslie off window to avoid hitting to another alien because of high framerate
            state = "rest";
            --Hp[i]; // alien got hit.. Hp -1
            if(Hp[i] == 0) {
                Hp.erase(Hp.begin()+i); // will remove the Hp of the Alien that was just killed
                aliens.erase(aliens.begin()+i); // it will remove the exact alien that we hit with our missile
                return true; // return true to make it known that we hit an alien
            }
        }
    }
    return false; // if the loop termiates normally.. no collision
}

bool canLaunch(vector<Missiles>& missiles){
    for(int i = 0; i<missiles.size(); i++){
        if(missiles[i].state == "rest"){
            missiles[i].state = "fire"; // launch the missile
            return true; // we can launch a missile
        }
    }
    return false;
}

void controlMissileStates(vector<Missiles>& missiles, float& missileY_change, vector<sf::Sprite>& aliens, sf:: Sprite& alien, vector<int>& Hp, int hp, sf::Sound& MissileCollision, sf::Sprite& spaceShip, float& deltaTime, int& score, string& musicPreference){
    for(int i = 0; i<missiles.size(); i++){
        if(missiles[i].state == "rest") missiles[i].setSprite.setPosition(spaceShip.getPosition().x+20, spaceShip.getPosition().y+20);
        else{ // the missile is launched
            missiles[i].setSprite.move(0, missileY_change*deltaTime);
            if(missiles[i].setSprite.getPosition().y <= 0){ // moves out of window.. bring to rest
                missiles[i].state= "rest";
            }
            else if(AlienMissileCollision(missiles[i].setSprite, aliens, missiles[i].state, Hp, MissileCollision, musicPreference)){
                // our missile did hit an alien.
                int needAlien = 1; // we removed one alien.. so, we will bring it back
                loadAliens(aliens, alien, needAlien); // it will bring back the ailens that we killed
                Hp.push_back(hp); // we are getting a new alien as prev was removed... so, we need new Hp for the new alien
                score++;
            }
        }
    }
}

void fillHp(vector<int>& Hp, int& hp, int size){
    for(int i = 0; i<size; i++){
        Hp.push_back(hp);
    }
}

void gameOver(sf::Text& GameOver, vector<Missiles>& missiles){
    for(int i = 0; i<missiles.size(); i++){
        missiles[i].state = "rest";
    }
    GameOver.setCharacterSize(60);
    GameOver.setFillColor(sf::Color::Magenta);
    GameOver.Bold;
    GameOver.setPosition(230, 300);
    GameOver.setString("Game Over!");
}


int main(){
    // I am connecting this to a text file...
    fstream fh, fhwrite;
    fh.open("HighScore.txt", ios::in); // opening the file in both read and write mode
    string highScore = "";
    char ch;
    while(fh.get(ch)){
        highScore += ch;
    }
    int highSco = 0;
    if(highScore != "") highSco = stoi(highScore); // storing in int format for comparision


    // creating a window 
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Space Invader");
    window.setKeyRepeatEnabled(false);
    sf::Clock clock;
    bool Over = false;

    // creating a font
    sf::Font font;
    font.loadFromFile("FreeRoyalty.ttf");
    sf::Text GameOver;
    GameOver.setFont(font);

    string onWhichScreen = "Menu"; // initially we will be in the menu screen
    // Different screens in this game will be : Menu ... Preference ... Game

    // BELOW: FOR MENU SCREEN
    // we will create 3 button for now... 1) Start Game... 2) Music On or Off... 3) Preference
    sf::RectangleShape startGame;
    startGame.setPosition(250, 150);
    startGame.setOutlineThickness(5);
    startGame.setOutlineColor(sf::Color::Red);
    startGame.setSize(sf::Vector2f(300, 60));

    sf::Text startGameText;
    startGameText.setFont(font);
    startGameText.setPosition(270, 150);
    startGameText.setCharacterSize(50);
    startGameText.setString("StartGame");
    startGameText.setFillColor(sf::Color::Green);

    // Button for Music Choice
    sf::RectangleShape musicOnOff;
    musicOnOff.setOutlineThickness(5);
    musicOnOff.setOutlineColor(sf::Color::Red);
    musicOnOff.setPosition(sf::Vector2f({250, 240}));
    musicOnOff.setSize(sf::Vector2f({300, 60}));

    sf::Text musicOption;
    string musicPreference = "On"; // yes for music on and no for music off
    musicOption.setFont(font);
    musicOption.setCharacterSize(50);
    musicOption.setFillColor(sf::Color::Green); // initially green for music On.
    musicOption.setPosition(sf::Vector2f({270, 240}));
    musicOption.setString("Music (On)");

    // creating a preference button
    sf::RectangleShape preferencesBox;
    preferencesBox.setOutlineColor(sf::Color::Red);
    preferencesBox.setOutlineThickness(5);
    preferencesBox.setPosition(sf::Vector2f(250, 330));
    preferencesBox.setSize(sf::Vector2f(300, 60));

    sf::Text PreferenceText;
    PreferenceText.setFont(font);
    PreferenceText.setCharacterSize(48);
    PreferenceText.setFillColor(sf::Color::Green);
    PreferenceText.setPosition(sf::Vector2f(270, 330));
    PreferenceText.setString("Preference");

    // Below For Our Preference Screen
    // in this screen we will allow user to choose the number of invaders and number of missiles he can fire

    // button for Number of invaders
    sf::RectangleShape invaderChoice;
    invaderChoice.setOutlineThickness(5);
    invaderChoice.setOutlineColor(sf::Color::Red);
    invaderChoice.setPosition(70, 50);
    invaderChoice.setSize(sf::Vector2f(350, 50));

    sf::Text invaderText;
    invaderText.setFont(font);
    invaderText.setFillColor(sf::Color::Magenta);
    invaderText.setCharacterSize(38);
    invaderText.setPosition(90, 50);
    // setstring will be done later in the code

    // box to increase invader count
    sf::RectangleShape increaseCount; 
    increaseCount.setOutlineThickness(5);
    increaseCount.setOutlineColor(sf::Color::Green);
    increaseCount.setPosition(450, 50);
    increaseCount.setSize(sf::Vector2f(50, 50));

    sf::Text increaseText;
    increaseText.setFont(font);
    increaseText.setCharacterSize(40);
    increaseText.setPosition(465, 50);
    increaseText.setFillColor(sf::Color::Blue);
    increaseText.setString("+");

    // box to decrease invader count
    sf::RectangleShape decreaseCount; 
    decreaseCount.setOutlineThickness(5);
    decreaseCount.setOutlineColor(sf::Color::Green);
    decreaseCount.setPosition(530, 50);
    decreaseCount.setSize(sf::Vector2f(50, 50));

    sf::Text decreaseText;
    decreaseText.setFont(font);
    decreaseText.setCharacterSize(40);
    decreaseText.setPosition(545, 50);
    decreaseText.setFillColor(sf::Color::Blue);
    decreaseText.setString("-");

    // now, option to increase the number of missiles one can fire
    sf::RectangleShape missileCountBox;
    missileCountBox.setOutlineThickness(5);
    missileCountBox.setOutlineColor(sf::Color::Red);
    missileCountBox.setPosition(70, 140);
    missileCountBox.setSize(sf::Vector2f(350, 50));

    sf::Text missileCountText;
    missileCountText.setFont(font);
    missileCountText.setCharacterSize(38);
    missileCountText.setPosition(105, 140);
    missileCountText.setFillColor(sf::Color::Magenta);
    // set text for it will be give later in the code

    // box to increase invader count
    sf::RectangleShape increaseMissiles; 
    increaseMissiles.setOutlineThickness(5);
    increaseMissiles.setOutlineColor(sf::Color::Green);
    increaseMissiles.setPosition(450, 140);
    increaseMissiles.setSize(sf::Vector2f(50, 50));

    sf::Text increaseMissileText;
    increaseMissileText.setFont(font);
    increaseMissileText.setCharacterSize(40);
    increaseMissileText.setPosition(465, 140);
    increaseMissileText.setFillColor(sf::Color::Blue);
    increaseMissileText.setString("+");

    // box to decrease invader count
    sf::RectangleShape decreaseMissiles; 
    decreaseMissiles.setOutlineThickness(5);
    decreaseMissiles.setOutlineColor(sf::Color::Green);
    decreaseMissiles.setPosition(530, 140);
    decreaseMissiles.setSize(sf::Vector2f(50, 50));

    sf::Text decreaseMissilesText;
    decreaseMissilesText.setFont(font);
    decreaseMissilesText.setCharacterSize(40);
    decreaseMissilesText.setPosition(545, 140);
    decreaseMissilesText.setFillColor(sf::Color::Blue);
    decreaseMissilesText.setString("-");

    // creating a back button to move to Menu screen form Preference
    sf::RectangleShape backButton;
    backButton.setOutlineColor(sf::Color::Red);
    backButton.setOutlineThickness(5);
    backButton.setPosition(600, 500);
    backButton.setSize(sf::Vector2f(140, 50));

    sf::Text back;
    back.setFont(font);
    back.setCharacterSize(40);
    back.setFillColor(sf::Color::Magenta);
    back.setPosition(620, 500);
    back.setString("Back");

    sf::RectangleShape normalModeBox;
    normalModeBox.setOutlineThickness(5);
    normalModeBox.setOutlineColor(sf::Color::Green);
    normalModeBox.setPosition(70, 230);
    normalModeBox.setSize(sf::Vector2f(160, 50));

    sf::Text normalModeText;
    normalModeText.setFont(font);
    normalModeText.setFillColor(sf::Color::Magenta);
    normalModeText.setCharacterSize(40);
    normalModeText.setPosition(78, 230);
    normalModeText.setString("Normal");

    // Button For Hard Mode
    sf::RectangleShape hardModeButton;
    hardModeButton.setOutlineThickness(5);
    hardModeButton.setOutlineColor(sf::Color::Red); // Color symbolises which mode is choosed
    hardModeButton.setPosition(270, 230);
    hardModeButton.setSize(sf::Vector2f(120, 50));
    
    sf::Text hardModeText;
    hardModeText.setFont(font);
    hardModeText.setFillColor(sf::Color::Magenta);
    hardModeText.setCharacterSize(40);
    hardModeText.setPosition(280, 230);
    hardModeText.setString("Hard");

    // Button for Nightmare mode
    sf::RectangleShape nightMareButton;
    nightMareButton.setOutlineThickness(5);
    nightMareButton.setOutlineColor(sf::Color::Red);
    nightMareButton.setPosition(430, 230);
    nightMareButton.setSize(sf::Vector2f(230, 50));

    sf::Text nightmareText;
    nightmareText.setFont(font);
    nightmareText.setCharacterSize(40);
    nightmareText.setFillColor(sf::Color::Magenta);
    nightmareText.setPosition(440, 230);
    nightmareText.setString("NightMare");

    string currMode = "Normal"; // other modes are Hard... NightMare (Modes can be changed only before startGame)
    bool gameStarted = false; // initially we are in Menu.. not StartGame

    // Below: FOR GAME SCREEN

    // Creating Sound that plays until the game runs
    
    sf::Music gamePlay;
    gamePlay.openFromFile("GamePlayAudio.wav");
    gamePlay.setLoop(true);
    gamePlay.setVolume(70);
    

    // creating sound for Missile Launch
    sf::SoundBuffer missileBuffer;
    missileBuffer.loadFromFile("missile.wav");
    sf::Sound missileLaunch;
    missileLaunch.setBuffer(missileBuffer);

    // Creating sound for Missile and Alien Collision
    sf::SoundBuffer collisionBuffer;
    collisionBuffer.loadFromFile("Explosion.wav");
    sf::Sound MissileCollision;
    MissileCollision.setBuffer(collisionBuffer);

    

    // score 
    int score = 0;
    sf::Text Score;
    Score.setFont(font);
    Score.setFillColor(sf::Color::Cyan);
    Score.setCharacterSize(30);
    Score.setPosition(10, 10);

    // highScore
    sf::Text HighScore;
    HighScore.setFont(font);
    HighScore.setCharacterSize(30);
    HighScore.setPosition(570, 0);
    HighScore.setFillColor(sf::Color::Green);
    HighScore.Bold;

    // Loading BackGround
    sf::Texture bgTexture;
    bgTexture.loadFromFile("Background.png");
    sf::Sprite backGround;
    backGround.setTexture(bgTexture);

    // creating a spaceShip
    sf::Texture shipTexture;
    shipTexture.loadFromFile("SpaceShip.png");
    sf::Sprite spaceShip;
    spaceShip.setTexture(shipTexture);
    spaceShip.setTextureRect(sf::IntRect(64, 0, 64, 64));
    spaceShip.setPosition(window.getSize().x/2-32, 536);
    float moveShipX = 550.0, moveShipY = 0; 

    // creating aliens
    sf::Texture alienTexture;
    alienTexture.loadFromFile("plunderReaper.png");
    sf::Sprite alien;
    alien.setTexture(alienTexture);
    vector<sf::Sprite> aliens; // will store aliens in a vector
    int totalAliens = 6;
    int minAliens = 1;
    int maxAliens = 20;
    vector<sf::Vector2f> AlienMovement; // will take note of each aliens movement.

    // time to throw bombs (missile)
    int totalMissiles = 2;
    int minMissiles = 1;
    int maxMissiles = 7;
    sf::Texture missileTexture;
    missileTexture.loadFromFile("missile00.png");
    sf::Sprite Missile;
    Missile.setTexture(missileTexture);
    Missiles missile; // one missile.. will be pushed in missilesy
    missile.setSprite = Missile;
    // loading all the missiles
    vector<Missiles> missiles(totalMissiles, missile); // will store all missiles
    float missileY_change = -600.0;

    // setting movement for all aliens
    setMovement(aliens, AlienMovement, totalAliens);
    loadAliens(aliens, alien, totalAliens); // loading aliens

    int hp = 1; 
    vector<int> Hp(totalAliens, hp); // initially no values
    // gameloop
    while(window.isOpen()){
        if(onWhichScreen == "Game") gameStarted = false; // game has started.. cannot switch modes now
        float deltaTime = clock.restart().asSeconds();
        invaderText.setString("Invader Count "+to_string(totalAliens));
        missileCountText.setString("Missile Count "+to_string(totalMissiles));
        
        if(onWhichScreen == "Game" && Over == false && musicPreference == "On"){
            if(gamePlay.getStatus() != sf::Music::Playing){ // music is not playing.. paused
                gamePlay.play(); // resume...
            }
        }
        else{
            if(gamePlay.getStatus() == sf::Music::Playing){
                gamePlay.pause();
            }
        }

        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }
            else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space && Over == false && onWhichScreen == "Game"){
                if(canLaunch(missiles)){
                    if(musicPreference == "On") missileLaunch.play();
                }
            }
            else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G && onWhichScreen == "Menu" && Over == false){
                onWhichScreen = "Game"; // move to game screen 
            }
            else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::M && (onWhichScreen == "Game" || onWhichScreen == "Preference") && Over == false){
                onWhichScreen = "Menu";
            }
            else if(onWhichScreen == "Menu" && event.type == sf::Event::MouseButtonPressed){
                if(event.mouseButton.button == sf::Mouse::Left){
                    if((sf::Mouse::getPosition(window).x > startGame.getPosition().x) && (sf::Mouse::getPosition(window).x < startGame.getPosition().x + startGame.getSize().x) && (sf::Mouse::getPosition(window).y > startGame.getPosition().y) && (sf::Mouse::getPosition(window).y < startGame.getPosition().y + startGame.getSize().y)){
                        onWhichScreen = "Game";
                    }
                    else if(onWhichScreen == "Menu" && (sf::Mouse::getPosition(window).x > musicOnOff.getPosition().x) && (sf::Mouse::getPosition(window).x < musicOnOff.getPosition().x + musicOnOff.getSize().x) && (sf::Mouse::getPosition(window).y > musicOnOff.getPosition().y) && (sf::Mouse::getPosition(window).y < musicOnOff.getPosition().y + musicOnOff.getSize().y)){
                        (musicPreference == "On")? musicPreference = "Off" : musicPreference = "On";
                        musicOption.setString("Music ("+musicPreference+")");
                    }
                    else if(onWhichScreen == "Menu" && (sf::Mouse::getPosition(window).x > preferencesBox.getPosition().x) && (sf::Mouse::getPosition(window).x < preferencesBox.getPosition().x + preferencesBox.getSize().x) && (sf::Mouse::getPosition(window).y > preferencesBox.getPosition().y) && (sf::Mouse::getPosition(window).y < preferencesBox.getPosition().y + preferencesBox.getSize().y)){
                        onWhichScreen = "Preference";
                    }
                }
            }
            else if(onWhichScreen == "Preference" && event.type == sf::Event::MouseButtonPressed && Over == false){
                if(event.mouseButton.button == sf::Mouse::Left){
                    if(totalAliens < maxAliens && (sf::Mouse::getPosition(window).x > increaseCount.getPosition().x) && (sf::Mouse::getPosition(window).x < increaseCount.getPosition().x + increaseCount.getSize().x) && (sf::Mouse::getPosition(window).y > increaseCount.getPosition().y) && (sf::Mouse::getPosition(window).y < increaseCount.getPosition().y + increaseCount.getSize().y)){
                        totalAliens += 1;
                        int needMore = 1;
                        Hp.push_back(hp); // hp for the other alien
                        loadAliens(aliens, alien, needMore);
                        setMovement(aliens, AlienMovement, needMore);
                        decreaseCount.setOutlineColor(sf::Color::Green);
                        if(totalAliens == maxAliens) increaseCount.setOutlineColor(sf::Color::Red);
                        
                    }
                    else if(totalAliens > minAliens && (sf::Mouse::getPosition(window).x > decreaseCount.getPosition().x) && (sf::Mouse::getPosition(window).x < decreaseCount.getPosition().x + decreaseCount.getSize().x) && (sf::Mouse::getPosition(window).y > decreaseCount.getPosition().y) && (sf::Mouse::getPosition(window).y < decreaseCount.getPosition().y + decreaseCount.getSize().y)){
                        totalAliens--;
                        Hp.pop_back(); // removing the last.. (not sure.. chatgpt review it)
                        aliens.pop_back(); // as it will remove the last added alien
                        AlienMovement.pop_back(); // removing movement for the last added alien
                        increaseCount.setOutlineColor(sf::Color::Green);
                        if(totalAliens == minAliens) decreaseCount.setOutlineColor(sf::Color::Red);
                    }
                    else if(totalMissiles < maxMissiles && (sf::Mouse::getPosition(window).x > increaseMissiles.getPosition().x) && (sf::Mouse::getPosition(window).x < increaseMissiles.getPosition().x + increaseMissiles.getSize().x) && (sf::Mouse::getPosition(window).y > increaseMissiles.getPosition().y) && (sf::Mouse::getPosition(window).y < increaseMissiles.getPosition().y + increaseMissiles.getSize().y)){
                        totalMissiles++;
                        missiles.push_back(missile); // we add another missile
                        decreaseMissiles.setOutlineColor(sf::Color::Green); // decrease option enabled
                        if(totalMissiles == 7) increaseMissiles.setOutlineColor(sf::Color::Red); // increase option disabled
                    }
                    else if(totalMissiles > minMissiles && (sf::Mouse::getPosition(window).x > decreaseMissiles.getPosition().x) && (sf::Mouse::getPosition(window).x < decreaseMissiles.getPosition().x + decreaseMissiles.getSize().x) && (sf::Mouse::getPosition(window).y > decreaseMissiles.getPosition().y) && (sf::Mouse::getPosition(window).y < decreaseMissiles.getPosition().y + decreaseMissiles.getSize().y)){
                        totalMissiles--;
                        missiles.pop_back(); // remove the last added missile
                        increaseMissiles.setOutlineColor(sf::Color::Green); // enable increase button
                        if(totalMissiles == minMissiles) decreaseMissiles.setOutlineColor(sf::Color::Red); // disable decrease button
                    }
                    else if((sf::Mouse::getPosition(window).x > backButton.getPosition().x) && (sf::Mouse::getPosition(window).x < backButton.getPosition().x + backButton.getSize().x) && (sf::Mouse::getPosition(window).y > backButton.getPosition().y) && (sf::Mouse::getPosition(window).y < backButton.getPosition().y + backButton.getSize().y)){
                        onWhichScreen = "Menu";
                    }
                    else if(gameStarted == false &&(sf::Mouse::getPosition(window).x > normalModeBox.getPosition().x) && (sf::Mouse::getPosition(window).x < normalModeBox.getPosition().x + normalModeBox.getSize().x) && (sf::Mouse::getPosition(window).y > normalModeBox.getPosition().y) && (sf::Mouse::getPosition(window).y < normalModeBox.getPosition().y + normalModeBox.getSize().y)){
                        currMode = "Normal";
                        hp = 1;
                        vector<int> tempHp;
                        fillHp(tempHp, hp, totalAliens);
                        Hp = tempHp; // to organize game properly.. as player cannot be restricted from choosing
                        missileY_change = -600.0; // increase missile speed to meet the difficulty
                        moveShipX = 550.0;
                        hardModeButton.setOutlineColor(sf::Color::Red);
                        nightMareButton.setOutlineColor(sf::Color::Red);
                        normalModeBox.setOutlineColor(sf::Color::Green);
                    }
                    else if(gameStarted == false && (sf::Mouse::getPosition(window).x > hardModeButton.getPosition().x) && (sf::Mouse::getPosition(window).x < hardModeButton.getPosition().x + hardModeButton.getSize().x) && (sf::Mouse::getPosition(window).y > hardModeButton.getPosition().y) && (sf::Mouse::getPosition(window).y < hardModeButton.getPosition().y + hardModeButton.getSize().y)){
                        currMode = "Hard";
                        hp = 2;
                        vector<int> tempHp;
                        fillHp(tempHp, hp, totalAliens);
                        Hp = tempHp; // to organize game properly.. as player cannot be restricted from choosing                        
                        missileY_change = -1000.0;
                        moveShipX = 800.0;
                        hardModeButton.setOutlineColor(sf::Color::Green);
                        nightMareButton.setOutlineColor(sf::Color::Red);
                        normalModeBox.setOutlineColor(sf::Color::Red);

                    }
                }
            }
        }

        if(onWhichScreen == "Game") moveShip(spaceShip, moveShipX, deltaTime, Over); // it will handle ship movements and bounds
        if(onWhichScreen == "Game") moveAliens(aliens, AlienMovement, deltaTime, Over);

        if(onWhichScreen == "Game") controlMissileStates(missiles, missileY_change, aliens, alien, Hp, hp, MissileCollision, spaceShip, deltaTime, score, musicPreference);
    
        if(Over == true){
            // state = "rest"; // forcefully bring the missile at rest... no need for this statement here as i have made changes in gameOver()
            gameOver(GameOver, missiles);
        }

        if(score > highSco) highSco = score;

        Score.setString("Score : "+to_string(score));
        HighScore.setString("High Score : "+to_string(highSco));

        window.clear(sf::Color::Black);
        if(onWhichScreen == "Game"){
            window.draw(backGround);
            displayMissiles(missiles, window);
            window.draw(spaceShip);
            displayAliens(aliens, window);
            window.draw(GameOver);
            window.draw(Score);
            window.draw(HighScore);
        }
        else if(onWhichScreen == "Menu"){
            window.draw(startGame);
            window.draw(startGameText);
            window.draw(musicOnOff);
            window.draw(musicOption);
            window.draw(preferencesBox);
            window.draw(PreferenceText);
        }
        else if(onWhichScreen == "Preference"){
            window.draw(invaderChoice);
            window.draw(invaderText);
            window.draw(increaseCount);
            window.draw(increaseText);
            window.draw(decreaseCount);
            window.draw(decreaseText);
            window.draw(missileCountBox);
            window.draw(missileCountText);
            window.draw(increaseMissiles);
            window.draw(increaseMissileText);
            window.draw(decreaseMissiles);
            window.draw(decreaseMissilesText);
            window.draw(normalModeBox);
            window.draw(normalModeText);
            window.draw(hardModeButton);
            window.draw(hardModeText);
            window.draw(nightMareButton);
            window.draw(nightmareText);
            window.draw(backButton);
            window.draw(back);
        }
        window.display();
    }
    fh.close();
    // after the end of program we will check if we got a new high score or not
    if(highScore == "" || (score > stoi(highScore))){
        fhwrite.open("HighScore.txt", ios::out);
        fhwrite<<score;
        fhwrite.close();
    }
}

