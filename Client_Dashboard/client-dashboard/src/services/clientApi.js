// Centralized API base for all client-related requests.
const API_BASE = "http://localhost:5001/api/Clients";

/*
 * Thin fetch wrapper that:
 * - applies JSON headers by default
 * - normalizes error handling
 * - safely handles 204 (no body)
 *
 * Keeps the rest of the app free from repetitive fetch/try/catch logic.
 */
async function request(url, options = {}) {
  const res = await fetch(url, {
    headers: {
      "Content-Type": "application/json",
      ...(options.headers || {})
    },
    ...options
  });

  if (!res.ok) {
    const text = await res.text();
    throw new Error(text || `Request failed with status ${res.status}`);
  }

  // 204 No Content
  if (res.status === 204) return null;
  return res.json();
}

/*
 * Fetch all clients.
 * Backend handles filtering archived records via query param.
 */
export async function getAllClients(includeArchived = false) {
  const url = includeArchived ? `${API_BASE}?includeArchived=true` : API_BASE;
  return request(url);
}

/*
 * Fetch a single client by id.
 */
export async function getClient(id) {
  return request(`${API_BASE}/${id}`);
}

/*
 * Create a new client.
 * Payload shape mirrors backend CreateClientRequest DTO.
 */
export async function createClient(payload) {
  return request(API_BASE, {
    method: "POST",
    body: JSON.stringify(payload)
  });
}

/*
 * Update an existing client.
 */
export async function updateClient(id, payload) {
  return request(`${API_BASE}/${id}`, {
    method: "PUT",
    body: JSON.stringify(payload)
  });
}

/*
 * Soft delete (archive).
 * Keeps historical data instead of removing rows.
 */
export async function archiveClient(id) {
  return request(`${API_BASE}/${id}/archive`, {
    method: "POST"
  });
}

/*
 * Unarchive a client (reverse soft delete).
 */
export async function unarchiveClient(id) {
  return request(`${API_BASE}/${id}/unarchive`, { method: "POST" });
}