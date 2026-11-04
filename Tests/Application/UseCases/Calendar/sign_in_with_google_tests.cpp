#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Calendar/SignInWithGoogle/sign_in_with_google_usecase.h"
#include "../../../Fakes/fake_calendar_auth_service.h"

TEST_CASE("SignInWithGoogleUseCase reports success when sign-in works") {
	FakeCalendarAuthService auth;
	SignInWithGoogleUseCase useCase(auth);

	SignInWithGoogleResponse response = useCase.Execute();

	CHECK(response.ok);
	CHECK(auth.signInCount == 1);
}

TEST_CASE("SignInWithGoogleUseCase reports failure when sign-in fails") {
	FakeCalendarAuthService auth;
	auth.signInSucceeds = false;
	SignInWithGoogleUseCase useCase(auth);

	SignInWithGoogleResponse response = useCase.Execute();

	CHECK_FALSE(response.ok);
	CHECK(auth.signInCount == 1);
}