#include "sign_out_of_google_usecase.h"

void SignOutOfGoogleUseCase::Execute() {
	this->authService.SignOut();
}