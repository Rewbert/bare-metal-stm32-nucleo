## What this is

This repository contains the companion code for a series of blog posts on bare metal programming of STM32 Nucleo boards, and Arm TrustZone-M, that can be found [here](https://www.krook.dev).

The repository contains several versions of the code, one for each of the blog posts. They are numbered accordingly. I will add direct links to the blog posts when they go live, so for now this is a "standalone" repository.

## Setup

Before any of the code in these folders can be built, `CMSIS` needs to be fetched. If you make `setup_cmsis.sh` executable you can run it to set it up correctly. You only need to do this once.

To flash your board, the blog post tells you to build `OpenOCD` from STs fork of it. You don't need to do this for the STM32L5 board, and at some point support for all the boards that the blog posts covers will make their way into upstream OpenOCD. Until then, if the upstream OpenOCD fails, you need to read the first blog posts description of how to set it up yourself. The `Makefile` in each of the directories will refer to a self-built OpenOCD installation, and you should modify it to point to your installation of OpenOCD.
