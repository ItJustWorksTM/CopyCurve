FROM registry.git.chalmers.se/courses/dit638/students/2021-group-09/deps/debian-builder:latest as builder
MAINTAINER ItJustWorksTM ItJustWorksTM@aerostun.dev
COPY --from=registry.git.chalmers.se/courses/dit638/students/2021-group-09/deps/debian-cmake:3.20.0 / /usr/local
COPY --from=registry.git.chalmers.se/courses/dit638/students/2021-group-09/deps/opendlv-video-h264-replay-headless:latest / /usr/local/bin

ADD . /opt/Sources
WORKDIR /opt/Sources

ARG COVERAGE
ARG ACCURACY
ENV PRINT_COVERAGE=yes

RUN cmake -P CMake/Scripts/DoBuild.cmake && \
    (if ! [ -z ${ACCURACY} ] || ! [ -z ${COVERAGE} ]; then cmake -P CMake/Scripts/RunTestsuite.cmake; fi) && \
    (if ! [ -z ${COVERAGE} ]; then cp build/coverage.xml /opt/copycurve-coverage.xml; fi) && \
    (if ! [ -z ${ACCURACY} ]; then cp -r build/test/results /opt/accuracy; fi) && \
    cp build/copycurve /opt/

FROM registry.git.chalmers.se/courses/dit638/students/2021-group-09/deps/debian-rt-base:latest
MAINTAINER ItJustWorksTM ItJustWorksTM@aerostun.dev
COPY --from=builder /opt/copycurve* /usr/local/bin/
ENTRYPOINT ["/usr/local/bin/copycurve"]
