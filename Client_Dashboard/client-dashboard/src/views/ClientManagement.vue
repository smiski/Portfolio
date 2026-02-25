<template>
  <div class="page">
    <header class="header">
      <div>
        <h1 class="title">Client Management</h1>
        <p class="subtitle">Edit client details, manage phone numbers, or archive.</p>
      </div>

      <div class="toolbar">
        <button class="btn secondary" type="button" @click="goBack">Back</button>
      </div>
    </header>

    <div v-if="error" class="alert" role="alert">
      {{ error }}
    </div>

    <div v-if="loading" class="muted">Loading...</div>

    <section v-else class="card">
      <div class="card-header">
        <h2 class="card-title">Details</h2>

        <div class="card-actions">
          <button class="btn ghost" type="button" @click="resetToLoaded" :disabled="saving || archiving">
            Reset
          </button>
        </div>
      </div>

      <div class="grid">
        <div class="field">
          <label class="label" for="clientName">Name</label>
          <input
            id="clientName"
            v-model.trim="form.name"
            type="text"
            class="input"
            autocomplete="off"
          />
        </div>

        <div class="field">
          <label class="label">Phone Numbers</label>

          <div v-for="(p, idx) in form.phoneNumbers" :key="idx" class="phone-row">
            <input
              v-model.trim="p.number"
              type="text"
              placeholder="e.g. 760-586-6759"
              class="input"
              autocomplete="off"
            />

            <button
              class="btn secondary"
              type="button"
              @click="removePhone(idx)"
              :disabled="form.phoneNumbers.length === 1"
              title="Remove phone number"
            >
              Remove
            </button>
          </div>

          <button class="btn secondary" type="button" @click="addPhone">
            Add phone
          </button>
        </div>
      </div>

      <div class="actions">
        <button class="btn secondary" type="button" @click="goBack" :disabled="saving || archiving">
          Cancel
        </button>

        <button class="btn" type="button" :disabled="saving || archiving" @click="save">
          {{ saving ? "Saving..." : "Save" }}
        </button>

        <button
          class="btn danger"
          type="button"
          :disabled="archiving || saving"
          @click="toggleArchive"
        >
        {{ archiving ? (isArchived ? "Unarchiving..." : "Archiving...") : (isArchived ? "Unarchive" : "Archive") }}
        </button>
      </div>
    </section>  
  </div>
</template>

<script>
import { getClient, updateClient, archiveClient, unarchiveClient } from "@/services/clientApi";

export default {
  name: "ClientManagement",
  props: ["id"],
  data() {
    return {
      loading: false,
      saving: false,
      archiving: false,
      error: "",

      clientId: null,
      loadedSnapshot: null,

      isArchived: false,

      form: {
        name: "",
        phoneNumbers: [],
      },
    };
  },
  async created() {
    // Support either explicit prop (router props: true) or reading from $route params.
    this.clientId = this.id || this.$route.params.id;
    await this.load();
  },
  methods: {
    async load() {
      this.error = "";
      this.loading = true;

      try {
        const client = await getClient(this.clientId);
        this.isArchived = !!client.isArchived;

        const normalized = {
          name: client.name || "",
          phoneNumbers: (client.phoneNumbers || []).map((p) => ({ number: p.number || "" })),
        };

        // Ensure the UI always renders at least one phone input row.
        if (normalized.phoneNumbers.length === 0) normalized.phoneNumbers.push({ number: "" });

        this.form = normalized;
        this.loadedSnapshot = JSON.parse(JSON.stringify(normalized));
      } catch (e) {
        this.error = e.message || "Failed to load client.";
      } finally {
        this.loading = false;
      }
    },

    resetToLoaded() {
      // Reset locally without re-fetching (fast + predictable).
      if (!this.loadedSnapshot) return;
      this.form = JSON.parse(JSON.stringify(this.loadedSnapshot));
      this.error = "";
    },

    addPhone() {
      this.form.phoneNumbers.push({ number: "" });
    },

    removePhone(idx) {
      this.form.phoneNumbers.splice(idx, 1);

      // Defensive: ensure never render an empty list of inputs.
      if (this.form.phoneNumbers.length === 0) {
        this.form.phoneNumbers.push({ number: "" });
      }
    },

    async save() {
      this.error = "";

      if (!this.form.name) {
        this.error = "Name is required.";
        return;
      }

      const payload = {
        name: this.form.name,
        phoneNumbers: (this.form.phoneNumbers || []).filter(
          (p) => p.number && p.number.trim().length > 0
        ),
      };

      this.saving = true;
      try {
        await updateClient(this.clientId, payload);
        this.goBack();
      } catch (e) {
        this.error = e.message || "Failed to update client.";
      } finally {
        this.saving = false;
      }
    },

    async toggleArchive() {
      this.error = "";
      this.archiving = true;

      try {
        if (this.isArchived) {
          // Client is currently archived -> unarchive it
          await unarchiveClient(this.clientId);
        } else {
          // Client is currently active -> archive it
          await archiveClient(this.clientId);
        }

        this.goBack();
      } catch (e) {
        this.error =
          e.message || (this.isArchived ? "Failed to unarchive client." : "Failed to archive client.");
      } finally {
        this.archiving = false;
      }
    },

    goBack() {
      this.$router.push({ name: "ClientDashboard" });
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
.toolbar{display:flex;align-items:center;gap:12px}

/* Card */
.card{border:1px solid #e4e7ec;border-radius:14px;padding:16px;background:#fff;box-shadow:0 1px 2px rgba(16,24,40,.06)}
.card-header{display:flex;align-items:center;justify-content:space-between;gap:12px;margin-bottom:12px}
.card-title{margin:0;font-size:18px;letter-spacing:-.01em}

/* Form */
.grid{display:grid;gap:14px}
.label{display:block;font-weight:600;margin-bottom:6px;color:#101828;font-size:14px}
.field{margin:0}

/* Keep inputs from overflowing their grid/cell */
.input{width:100%;max-width:100%;box-sizing:border-box;padding:10px 12px;border:1px solid #d0d5dd;border-radius:10px;outline:none;font-size:14px;transition:border-color .15s ease,box-shadow .15s ease}
.input:focus{border-color:#2c3e50;box-shadow:0 0 0 4px rgba(44,62,80,.12)}

.phone-row{display:grid;grid-template-columns:minmax(0,1fr) auto;gap:10px;align-items:center;margin-bottom:10px}
.actions{margin-top:14px;display:flex;justify-content:flex-end;gap:10px;flex-wrap:wrap}

/* Buttons */
.btn{padding:10px 14px;border-radius:10px;border:1px solid #2c3e50;background:#2c3e50;color:#fff;cursor:pointer;font-weight:600;font-size:14px;transition:transform .02s ease,opacity .2s ease,background .2s ease}
.btn:active{transform:translateY(1px)}
.btn:disabled{opacity:.6;cursor:not-allowed}
.btn.secondary{background:#fff;color:#2c3e50;border-color:#d0d5dd}
.btn.ghost{background:transparent;color:#344054;border-color:transparent}
.btn.danger{border-color:#b42318;background:#b42318}

/* Alerts / states */
.alert{color:#b42318;background:#fef3f2;border:1px solid #fecdca;padding:10px 12px;border-radius:12px;margin:12px 0 16px}
.muted{color:#667085;font-size:14px}
</style>