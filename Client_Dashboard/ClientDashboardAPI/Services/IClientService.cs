using System.Collections.Generic;
using System.Threading.Tasks;
using ClientDashboardAPI.Models;

namespace ClientDashboardAPI.Services
{
    // Defines client-related operations
    public interface IClientService
    {
        Task<ClientDto?> GetClientAsync(int id);
        // Returns all clients; archived clients are excluded unless specified
        Task<List<ClientDto>> GetAllClientsAsync(bool includeArchived = false);
        Task<int> CreateClientAsync(CreateClientRequest req);
        // Updates client details; returns false if client does not exist
        Task<bool> UpdateClientAsync(int id, UpdateClientRequest req);
        // Archives a client (soft delete); returns false if client does not exist
        Task<bool> ArchiveClientAsync(int id);
        // Reverses an archive (sets archived flag back to false). Returns false if not found.
        Task<bool> UnarchiveClientAsync(int id);
    }
}