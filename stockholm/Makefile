all:
	@docker build -t stockholm .
	@docker run --env-file .env stockholm

clean:
	@./clean.sh

re: clean all