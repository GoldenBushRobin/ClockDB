FROM alpine:latest AS builder
RUN apk add --no-cache build-base
RUN apk add --no-cache cmake make

COPY . /app

WORKDIR /app/build
RUN cmake ..
RUN make

FROM alpine:latest
COPY --from=builder /usr/lib/libstdc++.so.6 /usr/lib/libstdc++.so.6
COPY --from=builder /usr/lib/libgomp.so.1 /usr/lib/libgomp.so.1
COPY --from=builder /usr/lib/libgcc_s.so.1 /usr/lib/libgcc_s.so.1

WORKDIR /app
COPY --from=builder /app/build/build /app/build
COPY --from=builder /app/build/search /app/search
