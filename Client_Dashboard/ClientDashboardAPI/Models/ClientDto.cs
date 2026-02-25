using System.Collections.Generic;

namespace ClientDashboardAPI.Models
{
    // Read model representing a client and their phone numbers
    public class ClientDto
    {
        public int Id { get; set; }
        public string Name { get; set; } = "";
        public bool IsArchived { get; set; } // Soft delete flag used to exclude archived clients from default queries
        public List<PhoneNumberDto> PhoneNumbers { get; set; } = new List<PhoneNumberDto>();
    }
}