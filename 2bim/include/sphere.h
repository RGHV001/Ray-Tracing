#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"  // Inclui a defini��o da classe "hittable"
#include "vec3.h"      // Inclui a defini��o da classe "vec3"

class sphere : public hittable {
public:
    sphere() {}  // Construtor padr�o para a classe "sphere"
    sphere(point3 cen, double r, shared_ptr<material> m)
        : center(cen), radius(r), mat_ptr(m) {};  // Construtor com par�metros
    sphere(point3 cen, double r) : center(cen), radius(r) {};  // Construtor com par�metros

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;  // Fun��o de interse��o

public:
    point3 center;  // Centro da esfera
    double radius;  // Raio da esfera
    shared_ptr<material> mat_ptr;  // Ponteiro para o material da esfera
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;  // Vetor do centro da esfera � origem do raio
    auto a = r.direction().length_squared();  // Quadrado do comprimento da dire��o do raio
    auto half_b = dot(oc, r.direction());  // Metade do produto escalar entre oc e a dire��o do raio
    auto c = oc.length_squared() - radius * radius;  // Quadrado do comprimento de oc menos o quadrado do raio
    auto discriminant = half_b * half_b - a * c;  // Discriminante da equa��o quadr�tica

    if (discriminant < 0)  // Verifica se n�o h� interse��o com a esfera
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
    vec3 outward_normal = (rec.p - center) / radius;  // Normalizada para fora da esfera
    rec.set_face_normal(r, outward_normal);  // Define a normal e a orienta��o da face
    rec.mat_ptr = mat_ptr;  // Atribui o material � estrutura de registro de interse��o
    return true;  // Indica que houve interse��o
}

#endif
