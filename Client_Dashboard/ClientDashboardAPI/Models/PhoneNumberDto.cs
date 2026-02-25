namespace ClientDashboardAPI.Models
{
    // Represent a phone number associated with a client
    public class PhoneNumberDto
    {
        public int? Id { get; set; } // Optional identifier, used when returning or updating existing numbers
        public string Number { get; set; } = "";
    }
}