#ifndef DELETE_EVENT_USECASE_H
#define DELETE_EVENT_USECASE_H

#include "delete_event_request.h"
#include <memory>

// Forward declaration or include your calendar service/repository interface
class GoogleCalendarService; 

class DeleteEventUseCase {
public:
    explicit DeleteEventUseCase(std::shared_ptr<GoogleCalendarService> calendarService);
    
    void Execute(const DeleteEventRequest& request);

private:
    std::shared_ptr<GoogleCalendarService> calendarService_;
};

#endif
