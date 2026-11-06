#include "sign_in_with_google_usecase.h"

SignInWithGoogleResponse SignInWithGoogleUseCase::Execute() {
	return SignInWithGoogleResponse{ this->authService.SignIn() };
}