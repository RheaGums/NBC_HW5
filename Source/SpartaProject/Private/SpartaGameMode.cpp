// SpartaGameMode.cpp

#include "SpartaGameMode.h"
#include "SpartaCharacter.h"
#include "SpartaGameState.h"
#include "SpartaPlayerController.h"

ASpartaGameMode::ASpartaGameMode()
{
	DefaultPawnClass = ASpartaCharacter::StaticClass(); // 객체를 생성하진 않았지만 클래스를 반환해줌
	//클래스 정보를 넘겨서 반환을 해야 할때는 보통 StaticClass()를 많이 사용함.
	PlayerControllerClass = ASpartaPlayerController::StaticClass();
	
	GameStateClass = ASpartaGameState::StaticClass();
	
}
