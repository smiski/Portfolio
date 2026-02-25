using System.Collections.Generic;

namespace ClientDashboardAPI.Models
{
    // Request model for creating a new client
    public class CreateClientRequest
    {
        public string Name { get; set; } = "";
        public List<PhoneNumberDto> PhoneNumbers { get; set; } = new List<PhoneNumberDto>();
    }
}