#!bash
CLOUDFLARE_WORKER="### Change this value ###"
BOT_TOKEN="### Change this value ###"

curl -F "url=${CLOUDFLARE_WORKER}" "https://api.telegram.org/bot${BOT_TOKEN}/setWebhook"
