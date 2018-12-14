#pragma once

#include <GameEngine/Basics.h>

#include <GameEngine/Console/ConsoleFunction.h>
#include <GameEngine/GameApplication/WindowOutputTargetBase.h>
#include <GameEngine/GameState/GameState.h>

#include <Core/Application/Application.h>

#include <Foundation/Types/UniquePtr.h>

class ezWindowBase;
struct ezWindowCreationDesc;

class EZ_GAMEENGINE_DLL ezGameApplicationBase : public ezApplication
{
public:
  ezGameApplicationBase(const char* szAppName);

  /// \name Basics
  ///@{

public:
  /// \brief Returns the ezGameApplicationBase singleton
  static ezGameApplicationBase* GetGameApplicationBaseInstance() { return s_pGameApplicationBaseInstance; }

  /// \brief Calling this function requests that the application quits after the current invocation of Run() finishes.
  ///
  /// Can be overridden to prevent quitting under certain conditions.
  virtual void RequestQuit();

  /// \brief Returns whether RequestQuit() was called.
  EZ_ALWAYS_INLINE bool WasQuitRequested() const { return m_bWasQuitRequested; }

protected:
  static ezGameApplicationBase* s_pGameApplicationBaseInstance;

  bool m_bWasQuitRequested = false;

  ///@}
  /// \name Window Management
  ///@{

public:
  /// \brief Adds a top level window to the application.
  ///
  /// An output target is created for that window. Run() will call ezWindowBase::ProcessWindowMessages()
  /// on all windows that have been added.
  /// Most applications should add exactly one such window to the game application.
  /// Only few applications will add zero or multiple windows.
  ezWindowOutputTargetBase* AddWindow(ezWindowBase* pWindow);

  /// \brief Adds a top level window to the application with a custom output target.
  void AddWindow(ezWindowBase* pWindow, ezUniquePtr<ezWindowOutputTargetBase> pOutputTarget);

  /// \brief Removes a previously added window. Destroys its output target. Should be called at application shutdown.
  void RemoveWindow(ezWindowBase* pWindow);

  /// \brief Can be called by code that creates windows (e.g. an ezGameStateBase) to adjust or override settings, such as the window title
  /// or resolution.
  virtual void AdjustWindowCreation(ezWindowCreationDesc& desc) {}

  /// \brief Calls ezWindowBase::ProcessWindowMessages() on all known windows. Returns true, if any windows are available, at all.
  ///
  /// \note This should actually never be executed manually. It is only public for very specific edge cases.
  /// Otherwise this function is automatically executed once every frame.
  bool ProcessWindowMessages();

  /// \brief Returns the ezWindowOutputTargetBase object that is associated with the given window. The window must have been added via
  /// AddWindow()
  ezWindowOutputTargetBase* GetWindowOutputTarget(const ezWindowBase* pWindow) const;

  /// \brief Sets the ezWindowOutputTargetBase for a given window. The window must have been added via AddWindow()
  ///
  /// The previous ezWindowOutputTargetBase object (if any) will be destroyed.
  void SetWindowOutputTarget(const ezWindowBase* pWindow, ezUniquePtr<ezWindowOutputTargetBase> pOutputTarget);

protected:
  virtual ezUniquePtr<ezWindowOutputTargetBase> CreateWindowOutputTarget(ezWindowBase* pWindow) = 0;
  virtual void DestroyWindowOutputTarget(ezUniquePtr<ezWindowOutputTargetBase> pOutputTarget) = 0;

  struct WindowContext
  {
    ezWindowBase* m_pWindow;
    ezUniquePtr<ezWindowOutputTargetBase> m_pOutputTarget;
    bool m_bFirstFrame = true;
  };

  ezDynamicArray<WindowContext> m_Windows;

  ///@}
  /// \name Screenshots
  ///@{

public:
  /// \brief Schedules a screenshot to be taken at the end of the frame.
  ///
  /// After taking a screenshot, StoreScreenshot() is executed, which may decide where to write the result to.
  void TakeScreenshot();

protected:
  /// \brief Called with the result from taking a screenshot. The default implementation writes the image to disk at ':appdata/Screenshots'
  virtual void StoreScreenshot(const ezImage& image);

  void ExecuteTakeScreenshot(ezWindowOutputTargetBase* pOutputTarget);

  bool m_bTakeScreenshot = false;

  /// expose TakeScreenshot() as a console function
  ezConsoleFunction<void()> m_ConFunc_TakeScreenshot;

  ///@}
};
