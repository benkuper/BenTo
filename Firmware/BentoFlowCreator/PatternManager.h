#include "DeviceSettings.h" //For NUM_LEDS sharing

#define NUM_PATTERNS 14
#include "Patterns.h"

class PatternManager
{
public:
  enum PatternType { Snapshot, BatteryLevel, LedRepair, Red, RedGreen, Green,GreenBlue,Blue,BlueRed,White,Fish,FastFish, Checker,Checker2 };
  PatternType currentPatternType;

  Pattern * currentPattern;

  void update()
  {
    if(currentPattern == nullptr) nextPattern();

    if(currentPattern == nullptr)
    {
      #if SERIAL_DEBUG
      Serial.println("Current pattern null");
      #endif
      delay(100);
      return;
    }
    currentPattern->updatePattern();
    FastLED.show();
  }

  void nextPattern()
  {
    if(currentPattern == nullptr) setPattern(Snapshot);
    else  setPattern((PatternType)(int)((currentPatternType +1)%NUM_PATTERNS));

    #if SERIAL_DEBUG
    Serial.print("Next Pattern : ");
    Serial.print(currentPatternType);
    Serial.print(" not null = ");
    Serial.println(currentPattern != nullptr);
    #endif
  }

  void setPattern(PatternType type)
  {
    if(currentPattern != nullptr)
    {
      delete currentPattern;
      currentPattern = nullptr;
    }

    currentPatternType = (PatternType)min(max((int)type,0),NUM_PATTERNS-1);

     #if SERIAL_DEBUG
     Serial.print("Set pattern type : ");
     Serial.println(currentPatternType);
     #endif
     
     switch(currentPatternType)
     {
      case Snapshot:
      {
        #if SERIAL_DEBUG
        Serial.println("New Snapshot pattern !");
        #endif
        
        currentPattern = new SnapshotPattern();
        
      }
      break;

      case BatteryLevel:
      { currentPattern = new BatteryLevelPattern(); }
      break;

      case LedRepair:
      { currentPattern = new LedRepairPattern(); }
      break;

      case Red:
      { currentPattern = new SolidColorPattern(CRGB(127,0,0)); }
      break;

      case RedGreen:
      { currentPattern = new SolidColorPattern(CRGB(80,80,0)); }
      break;

      case Green:
      { currentPattern = new SolidColorPattern(CRGB(0,127,0)); }
      break;

      case GreenBlue:
      { currentPattern = new SolidColorPattern(CRGB(0,80,80)); }
      break;

      case Blue:
      { currentPattern = new SolidColorPattern(CRGB(0,0,127)); }
      break;

      case BlueRed:
      { currentPattern = new SolidColorPattern(CRGB(80,0,80)); }
      break;

      case White:
      { currentPattern = new SolidColorPattern(CHSV(0,0,127)); }
      break;

      case Fish:
      { currentPattern = new FishPattern(); }
      break;

      case FastFish:
      { currentPattern = new FishPattern(13,1); }
      break;

       case Checker:
      { currentPattern = new CheckerPattern(); }
      break;

       case Checker2:
      { currentPattern = new CheckerPattern(2,23,700,0); }
      break;
      
      default:
      {
      #if SERIAL_DEBUG
      Serial.print("Pattern not handled : ");
      Serial.print(currentPatternType);
      #endif
      }
      break;
     }

     if(currentPattern != nullptr)
     {        
        #if SERIAL_DEBUG
        Serial.print("Init pattern : ");
        Serial.println(currentPatternType);
        #endif
     }
  }

  #if USE_OSC
  boolean handleMessage(OSCMessage &msg, int offset)
  {

    int pOffset = msg.match("/pattern", offset);
    if(pOffset == 0) return false;
    
    if(msg.match("/set",offset+pOffset))
    {
      setPattern((PatternType) msg.getInt(0)); 
    }
    else
    {
      if(currentPattern != nullptr) currentPattern->handleMessage(msg, offset+pOffset);
    }

    return true;
  }
  #endif
};
