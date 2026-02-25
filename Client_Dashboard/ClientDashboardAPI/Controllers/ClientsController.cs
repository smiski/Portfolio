using System;
using System.Threading.Tasks;
using ClientDashboardAPI.Models;
using ClientDashboardAPI.Services;
using Microsoft.AspNetCore.Mvc;

namespace ClientDashboardAPI.Controllers
{
    /// <summary>
    /// Client management endpoints
    /// </summary>
    [ApiController]
    [Route("api/[controller]")]
    public class ClientsController : ControllerBase
    {
        private readonly IClientService _service;

        public ClientsController(IClientService service)
        {
            _service = service;
        }

        // Returns all clients. Archived clients are excluded by default
        [HttpGet]
        public async Task<IActionResult> GetAllClients([FromQuery] bool includeArchived = false)
        {
            var clients = await _service.GetAllClientsAsync(includeArchived);
            return Ok(clients);
        }

        // Returns a single client by ID
        [HttpGet("{id:int}")]
        public async Task<IActionResult> GetClient(int id)
        {
            var client = await _service.GetClientAsync(id);
            if (client == null) return NotFound();
            return Ok(client);
        }

        // Creates a new client and associated phone numbers
        [HttpPost]
        public async Task<IActionResult> CreateClient([FromBody] CreateClientRequest req)
        {
            try
            {
                var id = await _service.CreateClientAsync(req);
                var created = await _service.GetClientAsync(id);
                return CreatedAtAction(nameof(GetClient), new { id }, created);
            }
            catch (ArgumentException ex)
            {
                return BadRequest(new { error = ex.Message });
            }
        }

        // Updates client details - Phone numbers are replaced in full
        [HttpPut("{id:int}")]
        public async Task<IActionResult> UpdateClient(int id, [FromBody] UpdateClientRequest req)
        {
            try
            {
                var ok = await _service.UpdateClientAsync(id, req);
                if (!ok) return NotFound();
                var updated = await _service.GetClientAsync(id);
                return Ok(updated);
            }
            catch (ArgumentException ex)
            {
                return BadRequest(new { error = ex.Message });
            }
        }

        // Archives a client (soft delete)
        [HttpPost("{id:int}/archive")]
        public async Task<IActionResult> ArchiveClient(int id)
        {
            var ok = await _service.ArchiveClientAsync(id);
            if (!ok) return NotFound();
            return NoContent();
        }

        // Unarchives a client (reverses soft delete)
        [HttpPost("{id:int}/unarchive")]
        public async Task<IActionResult> UnarchiveClient(int id)
        {
            var ok = await _service.UnarchiveClientAsync(id);
            if (!ok) return NotFound();
            return NoContent();
        }
    }
}