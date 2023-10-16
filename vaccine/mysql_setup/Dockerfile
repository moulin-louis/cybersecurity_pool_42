FROM python:3.8-slim

# Install required packages and MariaDB client
RUN apt-get update \
    && apt-get install -y mariadb-client libmariadb-dev \
    && apt-get clean

# Set the working directory
WORKDIR /app

# Install Python dependencies
COPY ./tester/requirements.txt .
COPY ./tester/app.py .
COPY ./tester/main.html .
RUN pip install --no-cache-dir -r requirements.txt

# Expose the application port
EXPOSE 5000

# Start the Flask app
CMD ["python", "app.py"]