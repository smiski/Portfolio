<template>
  <div class="page">
    <section class="card">
      <h1 class="title">About This Project</h1>

      <p>
        My goal was not only to satisfy the acceptance criteria, but to structure the project
        the way I would approach a small production feature: clear separation of concerns,
        simple data flows, and maintainable code over cleverness.
      </p>

      <h2>Approach</h2>
      <p>
        I started by standing up the Docker environment and validating the backend first using
        Swagger. Establishing the API contract early allowed me to iterate on the database,
        service layer, and endpoints independently before introducing frontend complexity.
        Once the API was stable, the Vue client became a straightforward consumer of that contract.
      </p>

      <h2>Backend Design</h2>
      <ul>
        <li>Thin controllers focused only on HTTP concerns</li>
        <li>Business logic isolated behind an <code>IClientService</code> abstraction</li>
        <li>MySQL implementation kept simple and explicit (no hidden ORM behavior)</li>
        <li>Soft deletes (archive/unarchive) to avoid destructive operations</li>
      </ul>

      <p>
        I chose raw SQL with <code>MySqlConnector</code> for clarity and predictability.
        For a small domain like this, explicit queries are easy to reason about and avoid
        unnecessary abstraction overhead.
      </p>

      <h2>Frontend Design</h2>
      <ul>
        <li>Views handle presentation only</li>
        <li>API calls centralized in a small service layer</li>
        <li>Minimal state, simple forms, predictable flows</li>
        <li>Focused on readability and UX polish rather than heavy frameworks</li>
      </ul>

      <p>
        I intentionally kept components lightweight and avoided premature optimization.
        The goal was maintainability and straightforward behavior rather than architectural complexity.
      </p>

      <h2>Tradeoffs & Notes</h2>
      <p>
        For a take-home exercise, I prioritized clarity over scale concerns.
        For a larger system I would likely introduce pagination, validation layers,
        migrations, and stronger test coverage. The current structure keeps those
        additions easy to introduce without refactoring.
      </p>

      <h2>Closing Thoughts</h2>
      <p>
        I treated this as a small, real-world feature: ship something clean, readable,
        and easy for the next engineer to extend. I hope this reflects both my technical
        approach and how I think about maintainable software design. I thank you for the 
        opportunity to work on this exercise and look forward to any feedback you may have.
      </p>

      <p class="signature">
        — Caleb Mercier
      </p>
    </section>
  </div>
</template>

<style scoped>
.page {
  max-width: 900px;
  margin: 0 auto;
  padding: 32px 16px 60px;
  text-align: left;
}

.card {
  background: #fff;
  border: 1px solid #e4e7ec;
  border-radius: 14px;
  padding: 24px;
  box-shadow: 0 1px 2px rgba(16, 24, 40, 0.06);
}

.title {
  margin-top: 0;
  font-size: 28px;
  letter-spacing: -0.02em;
}

h2 {
  margin-top: 22px;
  font-size: 18px;
}

p {
  line-height: 1.6;
  color: #344054;
}

ul {
  margin: 8px 0 0 20px;
  color: #344054;
}

code {
  background: #f2f4f7;
  padding: 2px 6px;
  border-radius: 6px;
  font-size: 13px;
}

.signature {
  margin-top: 28px;
  font-weight: 600;
}
</style>