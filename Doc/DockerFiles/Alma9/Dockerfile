# syntax=docker/dockerfile:1
FROM picker24/genbox:alma9 AS build

# Add a labels
LABEL compiler="GNU 11.3.1"
LABEL root_version="v6.26.10"
LABEL org.opencontainers.image.description="Official Nuisance container"

# Declare the build argument
ARG NUISANCE3_VERSION
ENV NUISANCE3_VERSION=${NUISANCE3_VERSION:-main}

WORKDIR /opt/
RUN git clone https://github.com/NUISANCEMC/nuisance.git nuisance-src
RUN mkdir nuisance-build
WORKDIR /opt/nuisance-build
RUN cmake /opt/nuisance-src -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/opt/nuisance/git_master \
    && make install -j4

WORKDIR /opt/nuisance-src
RUN git rev-parse --short HEAD > /opt/nuisance/git_master/git.shorthash

FROM picker24/genbox:alma9
WORKDIR /

COPY --from=build /opt/nuisance/git_master /opt/nuisance/git_master

ENV NUISANCE=/opt/nuisance/git_master
ENV PATH=${NUISANCE}/bin:${PATH} \
    LD_LIBRARY_PATH=${NUISANCE}/lib:${NUISANCE}/lib64:${LD_LIBRARY_PATH}
