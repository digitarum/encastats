//
// Created by Long  on 18/11/2023.
//

// The main object will be a class that contains fields such as the state, the ID of the telegram
// chat that the bot uses, and the security token of the bot.

export default {
    chatID: '',             // read from the bot message
    bot_token: '',          // read from the worker environment variables
    KV: null,

    // This will receive the scheduled ping event
    async scheduled(event, env, ctx) {
        // You must have declared a Cloudflare KV with the following name:
        // * * * * * * * * * * * * * * * * * * * *
        this.KV = env.KV_ROVER; // * * * * * * * *
        // * * * * * * * * * * * * * * * * * * * *

        console.log("received a cron request");
        const state = await this.KV.get("state");
        if (state === "off") {
            console.log("The bot is off, no need to check if it is still alive");
            return null;
        }

        this.bot_token = env.BOT_TOKEN;
        this.chatID = env.CHAT_ID;

        await this.sendTelegramMessage("Worker executed a cron event");
        ctx.waitUntil(this.checkRoverAlive());
    },

    async fetch(request, env, ctx) {
        this.KV = env.KV_ROVER;

        // Handle preflight OPTIONS request - used to allow requests incoming
        // from the editor.swagger.io page
        if (request.method === 'OPTIONS') {
            return optionsRequest();
        }

        this.bot_token = env.BOT_TOKEN;
        if (this.bot_token === '') {
            return errNoToken();
        }

        const { pathname } = new URL(request.url);

        if (request.body == null) {
            return errNoBody();
        }

        const body = await request.json();
        this.chatID = extractChatIdFromBody(body);

        if (body && body.message && body.message.text) {
            console.log(`here is the chat text: ${body.message.text}`);
        } else {
            console.log("One of the objects is not defined.");
            await this.sendTelegramMessage("Could not read text of the message.")
            return new Response('Could not read text of the message');
        }

        return this.routeAndProcessRequest(body.message.text);
    },

    async routeAndProcessRequest(requestType) {
        switch(requestType) {
            case "state": case "return state":
                return this.returnState();
            case "turn on":
                return this.turnOn();
            case "turn off":
                return this.turnOff();
            case "ping":
                return this.ping();
            default:
                await this.sendTelegramMessage("I don't understand what you want.")
        }

        return new Response("I don't understand what you want.");
    },

    async returnState() {
        const now = new Date();
        const state = await this.KV.get("state");

        await this.sendTelegramMessage(`The rover is: ${state}. [${now.toLocaleString()}]`);
        return jsonResponse({"state": state, "message": `The rover is: ${state}`});
    },

    async turnOn() {
        await this.KV.put("state", "on");

        const timestamp = Date.now();
        const now = new Date(timestamp);

        await this.KV.put("last_ping_timestamp", timestamp)
        console.log(`I set the last ping timestamp to: ${timestamp}`);
        
        await this.sendTelegramMessage(`The rover was turned on [${now.toLocaleString()}]`);
        return jsonResponse({"state": "on", "message": "The rover was turned on"});
    },

    async turnOff() {
        const timestamp = Date.now();
        const now = new Date(timestamp);

        await this.KV.put("last_ping_timestamp", 0);
        await this.KV.put("state", "off");

        await this.sendTelegramMessage(`The rover was turned off [${now.toLocaleString()}]`);
        return jsonResponse({"state:": "off", "message": "The rover was turned off"});
    },

    async ping() {
        const timestamp = Date.now();
        const now = new Date();

        await this.KV.put("state", "on");
        await this.KV.put("last_ping_timestamp", timestamp);
        await this.sendTelegramMessage(`The rover sent a ping [${now.toLocaleString()}]`);
        return new Response("Ok");
    },

    async sendTelegramMessage(message) {
        const apiUrl = `https://api.telegram.org/bot${this.bot_token}/sendMessage`;

        try {
            console.log(`Fetching: ${apiUrl}`);
            const response = await fetch(apiUrl, {
                method: 'POST',
                headers: {  'Content-Type': 'application/json' },
                body: JSON.stringify({
                    chat_id: this.chatID,
                    text: message
                })
            })

            const responseBody = await response.text();
            console.log('Telegram API Response:', responseBody);
        } catch (e) {
            console.log("Something went wrong calling the Telegram API: " + e);
        }
    },

    async checkRoverAlive() {
        const lastPingTimestamp = await this.KV.get("last_ping_timestamp")

        console.log("check if rover is alive");
        console.log(`Last Ping Timestamp: ${lastPingTimestamp}`);

        const timestamp = Date.now();
        const now = new Date(timestamp);

        const inactivityDuration = timestamp - lastPingTimestamp;
        console.log(`Inactive for: ${inactivityDuration} milliseconds`);

        // Assuming 20 seconds as the threshold for inactivity (adjust as needed)
        if (inactivityDuration >= 20000) { // one minute, in milliseconds
            console.log("the rover has been inactive for more than 20 seconds");
            await this.KV.put("state", "off");
            await this.KV.put("last_ping_timestamp", 0);
            await this.sendTelegramMessage(`The bot was turned off after a period of inactivity [${now.toLocaleString()}]`);
            console.log("Robot state set to off due to inactivity");
        }
    }
}

// The following functions don't require any information from the worker

function jsonResponse(responseObject) {
    const jsonResponse = JSON.stringify(responseObject);

    return new Response(jsonResponse, {
        headers: {
            'Content-Type': 'application/json',
        },
    });
}

function extractChatIdFromBody(body) {
    return body.message && body.message.chat && body.message.chat.id;
}

async function errNoToken() {
    await this.sendTelegramMessage("Missing bot token. Check the configuration of your Cloudflare worker.");
}

async function errNoBody() {
    await this.sendTelegramMessage("Missing request body.");
}

function optionsRequest() {
    return new Response(null, {
        headers: {
            'Access-Control-Allow-Origin': '*',
            'Access-Control-Allow-Methods': 'GET, POST, OPTIONS',
            'Access-Control-Allow-Headers': 'Content-Type',
        },
    });
}
