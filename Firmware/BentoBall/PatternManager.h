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
      DBG("Current pattern null");
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

    DBG("Next Pattern : "+String((int)currentPatternType)+", not null ? "+String((int)(currentPattern != nullptr)));
  }

  void setPattern(PatternType type)
  {
    if(currentPattern != nullptr)
    {
      delete currentPattern;
      currentPattern = nullptr;
    }

    currentPatternType = (PatternType)min(max((int)type,0),NUM_PATTERNS-1);

     DBG("Set pattern type : "+String((int)currentPatternType));
     
     switch(currentPatternType)
     {
      case Snapshot:
      { currentPattern = new SnapshotPattern(); }
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
      DBG("Pattern not handled : "+String((int)currentPatternType));
      }
      break;
     }

     if(currentPattern != nullptr)
     {        
        DBG("Init pattern : "+String(currentPatternType));
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
