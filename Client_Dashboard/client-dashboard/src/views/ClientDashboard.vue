<template>
  <div class="page">
    <header class="header">
      <div>
        <h1 class="title">Client Dashboard</h1>
        <p class="subtitle">Manage clients and their phone numbers.</p>
      </div>

      <div class="toolbar">
        <!-- Toggle drives a query param to the API -->
        <label class="toggle" title="Include archived clients in the list">
          <input type="checkbox" v-model="showArchived" @change="loadClients" />
          <span>Show Archived</span>
        </label>

        <button class="btn" @click="toggleCreate">
          {{ showCreate ? "Close" : "Create New" }}
        </button>
      </div>
    </header>

    <div v-if="error" class="alert" role="alert">
      {{ error }}
    </div>

    <!-- Create Form -->
    <section v-if="showCreate" class="card">
      <div class="card-header">
        <h2 class="card-title">Create Client</h2>

        <div class="card-actions">
          <button class="btn ghost" @click="toggleCreate">Cancel</button>
        </div>
      </div>

      <div class="grid">
        <div class="field">
          <label class="label" for="clientName">Name</label>
          <input
            id="clientName"
            v-model.trim="createForm.name"
            type="text"
            placeholder="Client name"
            class="input"
            autocomplete="off"
          />
        </div>

        <div class="field">
          <label class="label">Phone Numbers</label>

          <div v-for="(p, idx) in createForm.phoneNumbers" :key="idx" class="phone-row">
            <input
              v-model.trim="p.number"
              type="text"
              placeholder="e.g. 760-586-6759"
              class="input"
              autocomplete="off"
            />

            <!-- Keep at least one row present to avoid an empty form state -->
            <button
              class="btn secondary"
              type="button"
              @click="removeCreatePhone(idx)"
              :disabled="createForm.phoneNumbers.length === 1"
              title="Remove phone number"
            >
              Remove
            </button>
          </div>

          <button class="btn secondary" type="button" @click="addCreatePhone">
            Add phone
          </button>
        </div>
      </div>

      <div class="actions">
        <button class="btn" :disabled="saving" @click="submitCreate">
          {{ saving ? "Saving..." : "Save" }}
        </button>
      </div>
    </section>

    <!-- Client List -->
    <section class="card">
      <div class="card-header">
        <h2 class="card-title">
          Clients
          <span class="count" v-if="!loading">({{ clients.length }})</span>
        </h2>
      </div>

      <div v-if="loading" class="muted">Loading...</div>

      <div v-else-if="clients.length === 0" class="empty">
        No clients found.
      </div>

      <ul v-else class="list">
        <li
          v-for="c in clients"
          :key="c.id"
          class="list-item"
          :class="{ archived: c.isArchived }"
          @click="goToClient(c.id)"
        >
          <div class="left">
            <div class="name">
              {{ c.name }}
              <span v-if="c.isArchived" class="badge">Archived</span>
            </div>
            <div class="meta">{{ (c.phoneNumbers || []).length }} phone(s)</div>
          </div>

          <div class="chevron" aria-hidden="true">›</div>
        </li>
      </ul>
    </section>
  </div>
</template>

<script>
import { getAllClients, createClient } from "@/services/clientApi";

const SHOW_ARCHIVED_KEY = "client-dashboard:showArchived";

export default {
  name: "ClientDashboard",
  data() {
    return {
      clients: [],
      loading: false,
      saving: false,
      error: "",
      showCreate: false,
      showArchived: false,
      createForm: {
        name: "",
        phoneNumbers: [{ number: "" }],
      },
    };
  },
  async created() {
    this.showArchived = localStorage.getItem(SHOW_ARCHIVED_KEY) === "true";
    await this.loadClients();
  },
  methods: {
    async loadClients() {
      this.error = "";
      this.loading = true;

      localStorage.setItem(SHOW_ARCHIVED_KEY, String(this.showArchived));

      try {
        this.clients = await getAllClients(this.showArchived);
      } catch (e) {
        this.error = e.message || "Failed to load clients.";
      } finally {
        this.loading = false;
      }
    },

    toggleCreate() {
      this.showCreate = !this.showCreate;
      this.error = "";

      // Reset the form when opening to keep the state predictable.
      if (this.showCreate) {
        this.createForm = { name: "", phoneNumbers: [{ number: "" }] };
      }
    },

    addCreatePhone() {
      this.createForm.phoneNumbers.push({ number: "" });
    },

    removeCreatePhone(idx) {
      this.createForm.phoneNumbers.splice(idx, 1);

      // Defensive: ensure never render an empty list of inputs.
      if (this.createForm.phoneNumbers.length === 0) {
        this.createForm.phoneNumbers.push({ number: "" });
      }
    },

    async submitCreate() {
      this.error = "";

      if (!this.createForm.name) {
        this.error = "Name is required.";
        return;
      }

      const payload = {
        name: this.createForm.name,
        phoneNumbers: (this.createForm.phoneNumbers || []).filter(
          (p) => p.number && p.number.trim().length > 0
        ),
      };

      this.saving = true;
      try {
        await createClient(payload);
        this.showCreate = false;
        await this.loadClients();
      } catch (e) {
        this.error = e.message || "Failed to create client.";
      } finally {
        this.saving = false;
      }
    },

    goToClient(id) {
      this.$router.push({ name: "ClientManagement", params: { id } });
    },
  },
};
</script>

<style scoped>
/* Layout */
.page{max-width:900px;margin:0 auto;padding:24px 16px 60px;text-align:left}
.header{display:flex;align-items:flex-end;justify-content:space-between;gap:16px;margin-bottom:18px}
.title{margin:0;font-size:28px;letter-spacing:-.02em}
.subtitle{margin:6px 0 0;color:#667085;font-size:14px}

/* Toolbar */
.toolbar{display:flex;align-items:center;gap:12px}
.toggle{display:inline-flex;align-items:center;gap:8px;padding:8px 10px;border:1px solid #e4e7ec;border-radius:10px;background:#fff;color:#344054;font-size:14px;user-select:none}
.toggle input{width:16px;height:16px}

/* Card */
.card{border:1px solid #e4e7ec;border-radius:14px;padding:16px;margin-bottom:16px;background:#fff;box-shadow:0 1px 2px rgba(16,24,40,.06)}
.card-header{display:flex;align-items:center;justify-content:space-between;gap:12px;margin-bottom:12px}
.card-title{margin:0;font-size:18px;letter-spacing:-.01em}
.count{color:#667085;font-weight:500;margin-left:6px}

/* Form */
.grid{display:grid;gap:14px}
.label{display:block;font-weight:600;margin-bottom:6px;color:#101828;font-size:14px}
.field{margin:0}

/* Keep inputs from overflowing their grid/cell */
.input{width:100%;max-width:100%;box-sizing:border-box;padding:10px 12px;border:1px solid #d0d5dd;border-radius:10px;outline:none;font-size:14px;transition:border-color .15s ease,box-shadow .15s ease}
.input:focus{border-color:#2c3e50;box-shadow:0 0 0 4px rgba(44,62,80,.12)}

.phone-row{display:grid;grid-template-columns:minmax(0,1fr) auto;gap:10px;align-items:center;margin-bottom:10px}
.actions{margin-top:14px;display:flex;justify-content:flex-end}

/* Buttons */
.btn{padding:10px 14px;border-radius:10px;border:1px solid #2c3e50;background:#2c3e50;color:#fff;cursor:pointer;font-weight:600;font-size:14px;transition:transform .02s ease,opacity .2s ease,background .2s ease}
.btn:active{transform:translateY(1px)}
.btn:disabled{opacity:.6;cursor:not-allowed}
.btn.secondary{background:#fff;color:#2c3e50;border-color:#d0d5dd}
.btn.ghost{background:transparent;color:#344054;border-color:transparent}

/* Alerts / states */
.alert{color:#b42318;background:#fef3f2;border:1px solid #fecdca;padding:10px 12px;border-radius:12px;margin:12px 0 16px}
.muted,.empty{color:#667085;font-size:14px}
.empty{padding:8px 0 2px}

/* List */
.list{list-style:none;padding:0;margin:0}
.list-item{padding:12px;border-top:1px solid #f2f4f7;cursor:pointer;display:flex;align-items:center;justify-content:space-between;border-radius:10px;transition:background .15s ease}
.list-item:hover{background:#f9fafb}
.left{display:flex;flex-direction:column;gap:4px;min-width:0}
.name{font-weight:700;color:#101828;display:inline-flex;align-items:center;gap:8px;min-width:0;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.meta{color:#667085;font-size:13px}
.chevron{color:#98a2b3;font-size:22px;line-height:1;padding-left:10px}
.badge{font-size:12px;padding:2px 8px;border:1px solid #d0d5dd;border-radius:999px;color:#344054;background:#fff}
.list-item.archived{opacity:.65}
</style>