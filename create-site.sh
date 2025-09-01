site="$1"
port="$2"

nginx_conf="/etc/nginx/sites-available/$site"
# nginx_conf="/root/C_projects/nginx-cli/$site"

cat > "$nginx_conf" <<EOF
server {
    listen 80;
    server_name $site;
    location / {
        proxy_pass http://localhost:$port;
    }
}
EOF

# ln -sf "$nginx_conf" "/etc/nginx/sites-enabled/$site"
nginx -t && systemctl reload nginx
