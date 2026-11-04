#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Calendar/GetGoogleSignInStatus/get_google_sign_in_status_usecase.h"
#include "../../../../Application/UseCases/Calendar/SignOutOfGoogle/sign_out_of_google_usecase.h"
#include "../../../Fakes/fake_calendar_auth_service.h"

TEST_CASE("GetGoogleSignInStatusUseCase reports signed out by default") {
	FakeCalendarAuthService auth;
	GetGoogleSignInStatusUseCase useCase(auth);

	CHECK_FALSE(useCase.Execute().signedIn);
}

TEST_CASE("GetGoogleSignInStatusUseCase reports signed in when the auth service is signed in") {
	FakeCalendarAuthService auth;
	auth.signedIn = true;
	GetGoogleSignInStatusUseCase useCase(auth);

	CHECK(useCase.Execute().signedIn);
}

TEST_CASE("SignOutOfGoogleUseCase signs out") {
	FakeCalendarAuthService auth;
	auth.signedIn = true;
	SignOutOfGoogleUseCase useCase(auth);

	useCase.Execute();

	CHECK(auth.signOutCount == 1);
	CHECK_FALSE(auth.signedIn);
}