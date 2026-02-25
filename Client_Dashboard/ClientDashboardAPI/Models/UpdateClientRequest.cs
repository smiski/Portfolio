using System.Collections.Generic;

namespace ClientDashboardAPI.Models
{
    // Request model for updating an existing client
    public class UpdateClientRequest
    {
        public string Name { get; set; } = "";
        public List<PhoneNumberDto> PhoneNumbers { get; set; } = new List<PhoneNumberDto>();
    }
}