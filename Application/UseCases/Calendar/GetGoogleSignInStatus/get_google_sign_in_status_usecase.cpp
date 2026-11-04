#include "get_google_sign_in_status_usecase.h"

GetGoogleSignInStatusResponse GetGoogleSignInStatusUseCase::Execute() {
	return GetGoogleSignInStatusResponse{ this->authService.IsSignedIn() };
}