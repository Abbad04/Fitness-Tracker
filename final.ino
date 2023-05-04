#include <M5Core2.h>
#include <String>

// Constants
const int sliderWidth = 200;
const int sliderHeight = 20;
const int sliderX = 60;
const int sliderY = 40; 
const int maxSliderValue = 1200;
const int touchThreshold = 50; 
const int slider2X = 60;
const int slider2Y = 140; 
const int maxSlider2Value = 2000;



// Global variables
int sliderValue = 0;
int slider2Value = 0;
float weight;
float height;
int walking_steps = 0;
int running_steps = 0;
float total = 0;
int count = 0;
float avg = 1.4;
float width = avg / 10;
boolean state = false;
boolean old_state = false;
int stationary_count = 0;

// Conversion
float walking_MET = 2.0;
float running_MET = 6.0;

float convert_MET = 3.5 / 200;
  // Calories burnt = (convert_MET)*(walking/running_MET)*(weight)*(time)

float walking_speed = 53.65;
float running_speed = 107.29;
  // Walking and running speeds above are in meters / min

float convert_walking_step = 0.42;
float convert_running_step = 0.45;
  // Time spent running/walking = (convert constant)(number of steps)(height in meters) / (walking/running speed)

//File names array
//String imageFileNames[4][7] = {{"/Char_1.1.jpg", "/Char_1.2.jpg", "/Char_1.3.jpg", "/Char_1.4.jpg", "/Char_1.5.jpg", "/Char_1.6.jpg", "/Char_1.7.jpg"},
//                               {"/Char_2.1.jpg", "/Char_2.2.jpg", "/Char_2.3.jpg", "/Char_2.4.jpg", "/Char_2.5.jpg", "/Char_2.6.jpg", "/Char_2.7.jpg"},
//                               {"/Char_3.1.jpg", "/Char_3.2.jpg", "/Char_3.3.jpg", "/Char_3.4.jpg", "/Char_3.5.jpg", "/Char_3.6.jpg", "/Char_3.7.jpg"},
//                               {"/Char_4.1.jpg", "/Char_4.2.jpg", "/Char_4.3.jpg", "/Char_4.4.jpg", "/Char_4.5.jpg", "/Char_4.6.jpg", "/Char_4.7.jpg"}};
                               

//Random character
int Rand;

// Setup function
void setup() {
  M5.begin();
  M5.Lcd.clear();
  randomSeed(20);
  Rand = random(0,4);   
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setTextSize(2);

  // Draw slider
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.print("Input your weight in kg:");
  M5.Lcd.drawRect(sliderX, sliderY, sliderWidth, sliderHeight, TFT_WHITE);
  updateSlider();

  // Draw slider 2
  M5.Lcd.setCursor(0, 100);
  M5.Lcd.print("Input your height in m:");
  M5.Lcd.drawRect(slider2X, slider2Y, sliderWidth, sliderHeight, TFT_WHITE);
  updateSlider();

  // Start touch event handling
  M5.Touch.begin();
  
  // Draw "OK" button
  M5.Lcd.fillRect(240, 180, 80, 40, TFT_GREEN);
  M5.Lcd.drawRect(240, 180, 80, 40, TFT_GREEN);
  M5.Lcd.setCursor(280 - 12, 200 - 8);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("OK");
  bool OK = false;
  while(OK == false){
    inputWeight();
    inputHeight();
    if (M5.Touch.ispressed()) {
      // Get touch coordinates
      int x = M5.Touch.getPressPoint().x;
      int y = M5.Touch.getPressPoint().y;
      if (x >= 240 && x <= 320 && y >= 180 && y <= 220) {
          M5.Lcd.fillScreen(BLACK);
          OK = true;
      }
    }
  }
  // IMU
  M5.IMU.Init();
}

void inputWeight(){
  M5.update();
  if (M5.Touch.ispressed()) {
    // Get touch coordinates
    int x = M5.Touch.getPressPoint().x;
    int y = M5.Touch.getPressPoint().y;
    // Check if touch is inside slider area
    if (x >= sliderX && x <= sliderX + sliderWidth && y >= sliderY - touchThreshold && y <= sliderY + sliderHeight + touchThreshold) {
      // Update slider value based on touch position
      sliderValue = map(x, sliderX, sliderX + sliderWidth, 0, maxSliderValue);
      updateSlider();
      // Output weight
      float w = round(sliderValue / 10.0);
      M5.Lcd.setCursor(80, 70);
      M5.Lcd.print(w);
      weight = w;
    }
  }
}

void inputHeight(){
  M5.update();
  if (M5.Touch.ispressed()) {
    // Get touch coordinates
    int x = M5.Touch.getPressPoint().x;
    int y = M5.Touch.getPressPoint().y;
    // Check if touch is inside slider area
    if (x >= slider2X && x <= slider2X + sliderWidth && y >= slider2Y - touchThreshold && y <= slider2Y + sliderHeight + touchThreshold) {
      // Update slider value based on touch position
      slider2Value = map(x, slider2X, slider2X + sliderWidth, 0, maxSlider2Value);
      updateSlider2();
      // Output height
      float h = round(slider2Value / 10.0);
      M5.Lcd.setCursor(80, 170);
      M5.Lcd.print(h);
      height = h;
    }
  }
}

void loop() {
  // Accelerometer value for x,y,z axis.
  float accX = 0;
  float accY = 0;
  float accZ = 0;

  // Get accel.
  M5.IMU.getAccelData(&accX, &accY, &accZ);
  float accel = sqrt(accX * accX + accY * accY + accZ * accZ);

  // Calibration for average acceleration.
  if (count < 11) {
    total += accel;
    count += 1;
  } else {
    avg = total / count;
    width = avg / 10;
    total = avg;
    count = 1;
  }

  // When current accel. is ...
  if (accel > avg + width * 1.25) {
    state = true;
  } else if (accel < avg - width) {
    state = false;
  }

  // Count up step and stationary count.
  if (!old_state && state) {
    if (accel > (avg + width * 3.75)) {
      // Running step
      running_steps += 1;
    } else {
      // Walking step
      walking_steps += 1;
    }
    
    // Display steps
//    M5.Lcd.setCursor(0, 0);
//    M5.Lcd.printf("Walking steps: %d\n", walking_steps);
//    M5.Lcd.setCursor(0, 40);
//    M5.Lcd.printf("Running steps: %d\n", running_steps);

    float caloriesTotal = calculateCaloriesBurned(walking_steps, running_steps, walking_speed, running_speed, walking_MET, running_MET, convert_walking_step, convert_running_step);

//    M5.Lcd.setCursor(0, 100);
//    M5.Lcd.printf("Weight: %f\n", weight);
//
//    M5.Lcd.setCursor(0, 120);
//    M5.Lcd.printf("height: %f\n", height);
//    
//    // Display calories burned
//    M5.Lcd.setCursor(0, 80);
//    M5.Lcd.printf("Calories burned: %f\n", caloriesTotal);

    //Dispay Character;
    M5.Lcd.setCursor(0, 140);
    displayChar(weight, height, caloriesTotal);
    
    // Reset stationary count when a step is taken.
    stationary_count = 0;
    delay(250);
    
  } else {
    // Increment stationary count when no step is taken.
    stationary_count += 1;
  }

  // Change background color to black when stationary.
  if (stationary_count >= 50) {
    M5.Lcd.fillScreen(BLACK);
  }
  
  old_state = state;
  delay(50);
}

// Calculate the total calories burned
float calculateCaloriesBurned(int walking_steps, int running_steps, float walking_speed, float running_speed, float walking_MET, float running_MET, float convert_walking_step, float convert_running_step) {

  // Calculate time spent walking/running
  float walking_time = (walking_steps * (height/100) * convert_walking_step) / walking_speed;
  float running_time = (running_steps * (height/100) * convert_running_step) / running_speed;

  // Calculate calories burned
  float caloriesWalk = convert_MET * walking_MET * weight * walking_time;
  float caloriesRun = convert_MET * running_MET * weight * running_time;
  float caloriesTotal = caloriesWalk + caloriesRun;
  
  return caloriesTotal;
}

// Function to update the slider display
void updateSlider() {
  // Clear slider area
  M5.Lcd.fillRect(sliderX + 1, sliderY + 1, sliderWidth - 2, sliderHeight - 2, TFT_BLACK);
  // Calculate slider fill width based on value
  int fillWidth = map(sliderValue, 0, maxSliderValue, 0, sliderWidth - 2);
  // Draw slider fill
  M5.Lcd.fillRect(sliderX + 1, sliderY + 1, fillWidth, sliderHeight - 2, TFT_WHITE);
}

// Function to update the slider2 display
void updateSlider2() {
  // Clear slider area
  M5.Lcd.fillRect(slider2X + 1, slider2Y + 1, sliderWidth - 2, sliderHeight - 2, TFT_BLACK);
  // Calculate slider fill width based on value
  int fillWidth = map(slider2Value, 0, maxSlider2Value, 0, sliderWidth - 2);
  // Draw slider fill
  M5.Lcd.fillRect(slider2X + 1, slider2Y + 1, fillWidth, sliderHeight - 2, TFT_WHITE);
}

// choosing a random character and displaying progress
void displayChar(float weight, float height, float caloriesTotal){
  String imageFileNames[4][7] = {{"/Char_1.1.jpg", "/Char_1.2.jpg", "/Char_1.3.jpg", "/Char_1.4.jpg", "/Char_1.5.jpg", "/Char_1.6.jpg", "/Char_1.7.jpg"},
                               {"/Char_2.1.jpg", "/Char_2.2.jpg", "/Char_2.3.jpg", "/Char_2.4.jpg", "/Char_2.5.jpg", "/Char_2.6.jpg", "/Char_2.7.jpg"},
                               {"/Char_3.1.jpg", "/Char_3.2.jpg", "/Char_3.3.jpg", "/Char_3.4.jpg", "/Char_3.5.jpg", "/Char_3.6.jpg", "/Char_3.7.jpg"},
                               {"/Char_4.1.jpg", "/Char_4.2.jpg", "/Char_4.3.jpg", "/Char_4.4.jpg", "/Char_4.5.jpg", "/Char_4.6.jpg", "/Char_4.7.jpg"}};
  if (height >= 160 || weight >= 50) {
    if (caloriesTotal >= 0 && caloriesTotal < 40) {
      const char* img = imageFileNames[Rand][0].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
    else if (caloriesTotal >= 40 && caloriesTotal < 62) {
      const char* img = imageFileNames[Rand][1].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
    else if (caloriesTotal >= 62 && caloriesTotal < 84) {
      const char* img = imageFileNames[Rand][2].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
    else if (caloriesTotal >= 84 && caloriesTotal < 106) {
      const char* img = imageFileNames[Rand][3].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
    else if (caloriesTotal >= 106 && caloriesTotal < 128) {
      const char* img = imageFileNames[Rand][4].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
    else if (caloriesTotal >= 128 && caloriesTotal < 150) {
      const char* img = imageFileNames[Rand][5].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
    else {
      const char* img = imageFileNames[Rand][6].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
  }
  else if (height >= 140 && height < 160) {
    if (caloriesTotal >= 0 && caloriesTotal < 30) {
      const char* img = imageFileNames[Rand][0].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
    else if (caloriesTotal >= 30 && caloriesTotal < 49) {
      const char* img = imageFileNames[Rand][1].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
    else if (caloriesTotal >= 49 && caloriesTotal < 68) {
      const char* img = imageFileNames[Rand][2].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
    else if (caloriesTotal >= 68 && caloriesTotal < 87) {
      const char* img = imageFileNames[Rand][3].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
    else if (caloriesTotal >= 87 && caloriesTotal < 106) {
      const char* img = imageFileNames[Rand][4].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
    else if (caloriesTotal >= 106 && caloriesTotal < 125) {
      const char* img = imageFileNames[Rand][5].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
    else {
      const char* img = imageFileNames[Rand][6].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
  }
  
  else if (height < 140) {
    if (caloriesTotal >= 0 && caloriesTotal <= 19) {
      const char* img = imageFileNames[Rand][0].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
    else if (caloriesTotal >= 20 && caloriesTotal < 36) {
      const char* img = imageFileNames[Rand][1].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
    else if (caloriesTotal >= 36 && caloriesTotal < 52) {
      const char* img = imageFileNames[Rand][2].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
    else if (caloriesTotal >= 52 && caloriesTotal < 68) {
      const char* img = imageFileNames[Rand][3].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
    else if (caloriesTotal >= 68 && caloriesTotal < 84) {
      const char* img = imageFileNames[Rand][4].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
    else if (caloriesTotal >= 84 && caloriesTotal < 100) {
      const char* img = imageFileNames[Rand][5].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
    else {
      const char* img = imageFileNames[Rand][6].c_str();
      M5.Lcd.drawJpgFile(SD, img);
    }
   }
}
