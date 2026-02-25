using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using ClientDashboardAPI.Models;
using Microsoft.Extensions.Configuration;
using MySqlConnector;

namespace ClientDashboardAPI.Services
{
    // MySQL-backed implementation of client-related operations
    public class MySqlClientService : IClientService
    {
        private readonly string _connectionString;

        public MySqlClientService(IConfiguration config)
        {
            // Prefer appsettings connection string; fall back to environment variables (Docker)
            _connectionString =
                config.GetConnectionString("Default")
                ?? BuildFromEnv()
                ?? throw new InvalidOperationException("Missing DB connection configuration.");
        }

        private static string? BuildFromEnv()
        {
            var host = Environment.GetEnvironmentVariable("DBHOST");
            var user = Environment.GetEnvironmentVariable("DBUSER") ?? "root";
            var pass = Environment.GetEnvironmentVariable("DBPASSWORD") ?? Environment.GetEnvironmentVariable("MYSQL_ROOT_PASSWORD");
            var db   = Environment.GetEnvironmentVariable("DBNAME") ?? Environment.GetEnvironmentVariable("MYSQL_DATABASE");
            var port = Environment.GetEnvironmentVariable("DBPORT") ?? "3306";

            if (string.IsNullOrWhiteSpace(host) || string.IsNullOrWhiteSpace(db))
                return null;

            var csb = new MySqlConnectionStringBuilder
            {
                Server = host,
                Port = UInt32.TryParse(port, out var p) ? p : 3306,
                Database = db,
                UserID = user,
                Password = pass ?? "",
                SslMode = MySqlSslMode.None,
                AllowPublicKeyRetrieval = true
            };
            return csb.ConnectionString;
        }

        private MySqlConnection NewConn() => new MySqlConnection(_connectionString);

        public async Task<ClientDto?> GetClientAsync(int id)
        {
            await using var conn = NewConn();
            await conn.OpenAsync();

            // Fetch client
            await using (var cmd = conn.CreateCommand())
            {
                cmd.CommandText = @"SELECT id, name, is_archived FROM Clients WHERE id = @id LIMIT 1;";
                cmd.Parameters.AddWithValue("@id", id);

                await using var reader = await cmd.ExecuteReaderAsync();
                if (!await reader.ReadAsync()) return null;

                var client = new ClientDto
                {
                    Id = reader.GetInt32("id"),
                    Name = reader.GetString("name"),
                    IsArchived = reader.GetBoolean("is_archived")
                };

                await reader.CloseAsync();

                // Fetch phone numbers
                client.PhoneNumbers = await GetPhoneNumbersAsync(conn, id);
                return client;
            }
        }

        public async Task<List<ClientDto>> GetAllClientsAsync(bool includeArchived = false)
        {
            var clients = new List<ClientDto>();

            await using var conn = NewConn();
            await conn.OpenAsync();

            await using (var cmd = conn.CreateCommand())
            {
                cmd.CommandText = includeArchived
                    ? @"SELECT id, name, is_archived FROM Clients ORDER BY id;"
                    : @"SELECT id, name, is_archived FROM Clients WHERE is_archived = 0 ORDER BY id;";

                await using var reader = await cmd.ExecuteReaderAsync();
                while (await reader.ReadAsync())
                {
                    clients.Add(new ClientDto
                    {
                        Id = reader.GetInt32("id"),
                        Name = reader.GetString("name"),
                        IsArchived = reader.GetBoolean("is_archived"),
                        PhoneNumbers = new List<PhoneNumberDto>()
                    });
                }
            }

            // Fetch phone numbers for each client
            foreach (var c in clients)
            {
                c.PhoneNumbers = await GetPhoneNumbersAsync(conn, c.Id);
            }

            return clients;
        }

        public async Task<int> CreateClientAsync(CreateClientRequest req)
        {
            if (string.IsNullOrWhiteSpace(req.Name))
                throw new ArgumentException("Client name is required.");

            await using var conn = NewConn();
            await conn.OpenAsync();
            await using var tx = await conn.BeginTransactionAsync();

            try
            {
                int clientId;

                // Insert client
                await using (var cmd = conn.CreateCommand())
                {
                    cmd.Transaction = tx;
                    cmd.CommandText = @"INSERT INTO Clients (name, is_archived) VALUES (@name, 0); SELECT LAST_INSERT_ID();";
                    cmd.Parameters.AddWithValue("@name", req.Name.Trim());
                    var result = await cmd.ExecuteScalarAsync();
                    clientId = Convert.ToInt32(result);
                }

                // Insert phone numbers
                await ReplacePhoneNumbersAsync(conn, tx, clientId, req.PhoneNumbers);

                await tx.CommitAsync();
                return clientId;
            }
            catch
            {
                await tx.RollbackAsync();
                throw;
            }
        }

        public async Task<bool> UpdateClientAsync(int id, UpdateClientRequest req)
        {
            if (string.IsNullOrWhiteSpace(req.Name))
                throw new ArgumentException("Client name is required.");

            await using var conn = NewConn();
            await conn.OpenAsync();
            await using var tx = await conn.BeginTransactionAsync();

            try
            {
                // Update client name
                int affected;
                await using (var cmd = conn.CreateCommand())
                {
                    cmd.Transaction = tx;
                    cmd.CommandText = @"UPDATE Clients SET name = @name WHERE id = @id;";
                    cmd.Parameters.AddWithValue("@name", req.Name.Trim());
                    cmd.Parameters.AddWithValue("@id", id);
                    affected = await cmd.ExecuteNonQueryAsync();
                }

                if (affected == 0)
                {
                    await tx.RollbackAsync();
                    return false;
                }

                // Replace phone numbers
                await ReplacePhoneNumbersAsync(conn, tx, id, req.PhoneNumbers);

                await tx.CommitAsync();
                return true;
            }
            catch
            {
                await tx.RollbackAsync();
                throw;
            }
        }

        public async Task<bool> ArchiveClientAsync(int id)
        {
            await using var conn = NewConn();
            await conn.OpenAsync();

            await using var cmd = conn.CreateCommand();
            cmd.CommandText = @"UPDATE Clients SET is_archived = 1 WHERE id = @id;";
            cmd.Parameters.AddWithValue("@id", id);

            var affected = await cmd.ExecuteNonQueryAsync();
            return affected > 0;
        }

        public async Task<bool> UnarchiveClientAsync(int id)
        {
            await using var conn = NewConn();
            await conn.OpenAsync();

            await using var cmd = conn.CreateCommand();

            // Soft-delete reversal
            cmd.CommandText = @"UPDATE Clients SET is_archived = 0 WHERE id = @id;";
            cmd.Parameters.AddWithValue("@id", id);

            var affected = await cmd.ExecuteNonQueryAsync();
            return affected > 0;
        }

        private static async Task<List<PhoneNumberDto>> GetPhoneNumbersAsync(MySqlConnection conn, int clientId)
        {
            var phones = new List<PhoneNumberDto>();

            await using var cmd = conn.CreateCommand();
            cmd.CommandText = @"SELECT id, phone_number FROM PhoneNumbers WHERE client_id = @clientId ORDER BY id;";
            cmd.Parameters.AddWithValue("@clientId", clientId);

            await using var reader = await cmd.ExecuteReaderAsync();
            while (await reader.ReadAsync())
            {
                phones.Add(new PhoneNumberDto
                {
                    Id = reader.GetInt32("id"),
                    Number = reader.GetString("phone_number")
                });
            }

            return phones;
        }

        private static async Task ReplacePhoneNumbersAsync(MySqlConnection conn, MySqlTransaction tx, int clientId, List<PhoneNumberDto> phoneNumbers)
        {
            // Delete existing
            await using (var del = conn.CreateCommand())
            {
                del.Transaction = tx;
                del.CommandText = @"DELETE FROM PhoneNumbers WHERE client_id = @clientId;";
                del.Parameters.AddWithValue("@clientId", clientId);
                await del.ExecuteNonQueryAsync();
            }

            // Insert new (skip blanks)
            foreach (var p in phoneNumbers)
            {
                var num = (p.Number ?? "").Trim();
                if (string.IsNullOrWhiteSpace(num)) continue;

                await using var ins = conn.CreateCommand();
                ins.Transaction = tx;
                ins.CommandText = @"INSERT INTO PhoneNumbers (client_id, phone_number) VALUES (@clientId, @num);";
                ins.Parameters.AddWithValue("@clientId", clientId);
                ins.Parameters.AddWithValue("@num", num);
                await ins.ExecuteNonQueryAsync();
            }
        }
    }
}