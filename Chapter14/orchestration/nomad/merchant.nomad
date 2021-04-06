job "merchant" {
  datacenters = ["dc1"]
  type = "service"
  group "merchant" {
    count = 3
    task "merchant" {
      driver = "docker"
      config {
        image = "hosacpp/merchant:v2.0.3"
        port_map {
          http = 8000
        }
      }
      resources {
        network {
          port "http" {
            static = 8000
          }
        }
      }
      service {
        name = "merchant"
        tags = ["dominican-front", "merchant"]
        port = "http"
        check {
          type = "tcp"
          interval = "10s"
          timeout = "2s"
        }
      }
    }
  }
}
