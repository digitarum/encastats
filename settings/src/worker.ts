/**
 * Welcome to Cloudflare Workers! This is your first worker.
 *
 * - Run "npm run dev" in your terminal to start a development server
 * - Open a browser tab at http://localhost:8787/ to see your worker in action
 * - Run "npm run deploy" to publish your worker
 *
 * Learn more at https://developers.cloudflare.com/workers/
 */

export default {
  async fetch(request, env, ctx) {
    const responseObject = {
      ssid: env.SSID,
      password: env.PASSWORD,
      chatId: env.CHAT_ID,
    };

    // Convert the JSON object to a string
    const responseBody = JSON.stringify(responseObject);

    // Return a JSON response
    return new Response(responseBody, {
      headers: {
        'Content-Type': 'application/json',
      },
    });
  },
};