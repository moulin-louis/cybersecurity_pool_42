# Use the official PHP Apache image as the base
FROM php:7.4-apache

# Install system dependencies
RUN apt-get update && apt-get install -y \
    git \
    libfreetype6-dev \
    libjpeg62-turbo-dev \
    libpng-dev \
    libzip-dev \
    libonig-dev \
    libxml2-dev \
    mariadb-server \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Configure and install PHP extensions
RUN docker-php-ext-configure gd --with-freetype --with-jpeg \
    && docker-php-ext-install -j$(nproc) gd \
    && docker-php-ext-install mysqli pdo_mysql mbstring zip exif pcntl xml bcmath

# Enable mod_rewrite for Apache
RUN a2enmod rewrite

# Adjust MariaDB configuration to allow outside connections
RUN sed -i 's/bind-address\s*=\s*127.0.0.1/bind-address = 0.0.0.0/' /etc/mysql/mariadb.conf.d/50-server.cnf && \
     service mariadb start && mariadb -uroot -e "GRANT ALL PRIVILEGES ON *.* TO 'root'@'localhost'"

# Copy SQLi Lab source to the appropriate directory in the container
COPY sqli-labs/ /var/www/html/

# Set appropriate permissions
RUN chown -R www-data:www-data /var/www/html && chmod -R 755 /var/www/html/

# Expose port 80 for Apache and 3306 for MariaDB
EXPOSE 80 3306

# Add a script to run MariaDB commands
COPY ./init-mariadb.sh /tmp
RUN chmod +x /tmp/init-mariadb.sh

# Starting services
CMD service mariadb start && /tmp/init-mariadb.sh && apache2-foreground