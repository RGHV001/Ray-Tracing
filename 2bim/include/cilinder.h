#ifndef CILINDER_H
#define CILINDER_H

#include "hittable.h"
#include "vec3.h"

class cilinder : public hittable {
public:
    cilinder() {}
    cilinder(point3 base_center, double height, double radius, shared_ptr<material> m)
        : base_center(base_center), height(height), radius(radius), mat_ptr(m) {};
    cilinder(point3 base_center, double height, double radius)
        : base_center(base_center), height(height), radius(radius) {};

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

public:
    point3 base_center;  // Centro da base do cilindro
    double height;       // Altura do cilindro
    double radius;       // Raio do cilindro
    shared_ptr<material> mat_ptr;  // Ponteiro para o material do cilindro
};

bool cilinder::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - base_center;  // Vetor da base do cilindro � origem do raio
    auto a = r.direction().length_squared() - dot(r.direction(), vec3(0, 1, 0)) * dot(r.direction(), vec3(0, 1, 0));  // Quadrado do comprimento da dire��o do raio (desconsiderando a componente y)
    auto half_b = dot(oc, r.direction()) - dot(oc, vec3(0, 1, 0)) * dot(r.direction(), vec3(0, 1, 0));  // Metade do produto escalar entre oc e a dire��o do raio (desconsiderando a componente y)
    auto c = oc.length_squared() - radius * radius - dot(oc, vec3(0, 1, 0)) * dot(oc, vec3(0, 1, 0));  // Quadrado do comprimento de oc menos o quadrado do raio (desconsiderando a componente y)
    auto discriminant = half_b * half_b - a * c;  // Discriminante da equa��o quadr�tica

    if (discriminant < 0)  // Verifica se n�o h� interse��o com o cilindro
        return false;

    auto sqrtd = sqrt(discriminant);  // Raiz quadrada do discriminante

    // Encontra a raiz mais pr�xima que est� no intervalo aceit�vel.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;  // Atribui o par�metro t do ponto de interse��o
    rec.p = r.at(rec.t);  // Calcula o ponto de interse��o no raio

    // Verifica se o ponto de interse��o est� dentro da altura do cilindro
    if (rec.p.y() < base_center.y() || rec.p.y() > base_center.y() + height)
        return false;

    // Calcula a normal do ponto de interse��o
    vec3 direction = vec3(base_center.x(), rec.p.y(), base_center.z()) - rec.p;
    vec3 outward_normal = direction / (radius * radius);
    rec.set_face_normal(r, outward_normal);  // Define a normal e a orienta��o da face
    rec.mat_ptr = mat_ptr;  // Atribui o material � estrutura de registro de interse��o
    return true;  // Indica que houve interse��o
}

#endif
