#ifndef PPM_INCLUDE_PPACK_TIMER_H
#define PPM_INCLUDE_PPACK_TIMER_H

#include <chrono>


#ifdef PUMPKIN_ROLL_LIB
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif


namespace pPack {

class Timer {
  double totalTime;
  double deltaTime;
  double timeScale;
  size_t updateDelay;

  std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> startPoint;


public:

  API static Timer& GetSingleton();



  API void Advance();
  API void Advance(double delta);

  API double GetDeltaTime() const;
  API double GetTimeScale() const;
  API size_t GetUpdateDelay() const;
  API double GetTotalTime() const;

  void SetTimeScale(double Scale);
  void SetUpdateDelay(double Rate);

  API void WaitForNextUpdate();
  API void ResetDeltaTime();


  API Timer();

};

}; // namespace pPack

#undef API

#endif