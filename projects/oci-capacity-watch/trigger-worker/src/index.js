async function dispatchToGitHub(env) {
  const repository = env.GITHUB_REPOSITORY;
  const eventType = env.GITHUB_EVENT_TYPE || "oci-capacity-watch";
  const token = env.GITHUB_TOKEN;

  if (!repository) throw new Error("GITHUB_REPOSITORY is not configured");
  if (!token) throw new Error("GITHUB_TOKEN is not configured");

  const response = await fetch(`https://api.github.com/repos/${repository}/dispatches`, {
    method: "POST",
    headers: {
      Accept: "application/vnd.github+json",
      Authorization: `Bearer ${token}`,
      "Content-Type": "application/json",
      "User-Agent": "oci-capacity-watch-trigger",
      "X-GitHub-Api-Version": "2026-03-10",
    },
    body: JSON.stringify({ event_type: eventType }),
  });

  if (response.status !== 204) {
    const body = (await response.text()).slice(0, 500);
    throw new Error(`GitHub repository_dispatch failed: HTTP ${response.status} ${body}`);
  }

  console.log(`repository_dispatch sent: ${repository} / ${eventType}`);
}

export default {
  async scheduled(_controller, env, ctx) {
    ctx.waitUntil(dispatchToGitHub(env));
  },
};
